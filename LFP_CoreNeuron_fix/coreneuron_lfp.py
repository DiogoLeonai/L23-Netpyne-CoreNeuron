import os
from array import array

import numpy as np
import sys
from neuron import h

def _section_type(sec_name):
    """
    Convert NetPyNE section names to CoreNEURON section classes.
    """

    name = sec_name.lower()

    if name.startswith('soma'):
        return 'soma'

    if name.startswith('apic'):
        return 'apic'

    if name.startswith('dend'):
        return 'dend'

    if name.startswith('myelin'):
        return 'myelin'

    if name.startswith('ais'):
        return 'ais'

    if name.startswith('node'):
        return 'node'

    if name.startswith('axon'):
        return 'axon'

    return 'cell'

def register_core_lfp_mappings():
    """
    Register NetPyNE's extracellular transfer resistances as
    CoreNEURON native LFP factors.

    Must be called after NetPyNE has calculated:
        sim.net.recXElectrode.transferResistances
    """

    from .. import sim

    n_electrodes = len(sim.cfg.recordLFP)

    if n_electrodes == 0:
        return

    if not hasattr(sim.pc, 'nrnbbcore_register_mapping'):
        raise RuntimeError(
            'CoreNEURON LFP requires '
            'ParallelContext.nrnbbcore_register_mapping()'
        )

    # These features still depend on NetPyNE's old current gathering.
    # Keep the first implementation simple.
    if getattr(sim.cfg, 'saveIMembrane', False):
        raise NotImplementedError(
            'saveIMembrane is not yet supported with native CoreNEURON LFP'
        )

    if getattr(sim.cfg, 'saveLFPCells', False):
        raise NotImplementedError(
            'saveLFPCells is not yet supported with native CoreNEURON LFP'
        )

    if getattr(sim.cfg, 'saveLFPPops', False):
        raise NotImplementedError(
            'saveLFPPops is not yet supported with native CoreNEURON LFP'
        )

    # Probably not strictly necessary because register_mapping copies
    # the vectors internally, but keeping references is cheap and safe.
    sim._coreLFP_mapping_refs = []

    local_cells = 0
    local_segments = 0

    for cell in sim.net.compartCells:

        gid = int(cell.gid)

        # ----------------------------------------------------------
        # NetPyNE transfer resistance
        #
        # shape:
        #     electrodes x segments
        # ----------------------------------------------------------

        tr = np.asarray(
            sim.net.recXElectrode.getTransferResistance(gid),
            dtype=np.float64
        )

        if tr.ndim != 2:
            raise RuntimeError(
                f'[CoreLFP] gid={gid}: '
                f'expected 2-D transfer matrix, got {tr.shape}'
            )

        if tr.shape[0] != n_electrodes:
            raise RuntimeError(
                f'[CoreLFP] gid={gid}: '
                f'transfer matrix has {tr.shape[0]} electrodes, '
                f'but recordLFP has {n_electrodes}'
            )

        if not np.all(np.isfinite(tr)):
            raise RuntimeError(
                f'[CoreLFP] gid={gid}: '
                'transfer matrix contains NaN/Inf'
            )

        # ----------------------------------------------------------
        # Build mapping using the SAME segment traversal order
        # used by NetPyNE.
        #
        # Each mapping entry contains:
        #
        #   section ID
        #   NEURON node index
        #   column in NetPyNE transfer-resistance matrix
        # ----------------------------------------------------------

        groups = {}

        jseg = 0

        for section_id, (sec_name, sec_data) in enumerate(
            cell.secs.items()
        ):

            hsec = sec_data.get('hObj', None)

            if hsec is None:
                continue

            sec_type = _section_type(sec_name)

            if sec_type not in groups:
                groups[sec_type] = {
                    'sections': [],
                    'segments': [],
                    'tr_columns': [],
                }

            for seg in hsec:

                groups[sec_type]['sections'].append(
                    section_id
                )

                groups[sec_type]['segments'].append(
                    int(seg.node_index())
                )

                groups[sec_type]['tr_columns'].append(
                    jseg
                )

                jseg += 1

        # ----------------------------------------------------------
        # Critical validation
        # ----------------------------------------------------------

        if jseg != tr.shape[1]:
            raise RuntimeError(
                f'[CoreLFP] gid={gid}: segment mismatch. '
                f'NetPyNE transfer matrix has {tr.shape[1]} segments, '
                f'but CoreLFP traversal found {jseg}.'
            )

        # ----------------------------------------------------------
        # Register each section class
        # ----------------------------------------------------------

        for sec_type, mapping in groups.items():

            columns = np.asarray(
                mapping['tr_columns'],
                dtype=np.int64
            )

            # NetPyNE:
            #
            #       electrode x segment
            #
            # CoreNEURON wants:
            #
            #       segment 0: e0,e1,e2
            #       segment 1: e0,e1,e2
            #       ...
            #
            factors = (
                tr[:, columns]
                .T
                .reshape(-1)
            )

            sec_vec = h.Vector(
                mapping['sections']
            )

            seg_vec = h.Vector(
                mapping['segments']
            )

            factor_vec = h.Vector(
                factors.tolist()
            )

            sim.pc.nrnbbcore_register_mapping(
                gid,
                sec_type,
                sec_vec,
                seg_vec,
                factor_vec,
                n_electrodes
            )

            sim._coreLFP_mapping_refs.append(
                (
                    sec_vec,
                    seg_vec,
                    factor_vec
                )
            )

        local_cells += 1
        local_segments += jseg

    # CoreNEURON LFP reports enable fast imem themselves,
    # but making NetPyNE's intention explicit is useful.
    sim.cfg.use_fast_imem = True
    sim.cvode.use_fast_imem(True)

    global_cells = int(
        sim.pc.allreduce(local_cells, 1)
    )

    global_segments = int(
        sim.pc.allreduce(local_segments, 1)
    )

    if sim.rank == 0:
        print(
            '[CoreLFP] Registered native CoreNEURON mappings: '
            f'{global_cells} cells, '
            f'{global_segments} segments, '
            f'{n_electrodes} electrodes',
            flush=True
        )


def _get_all_compartment_gids():
    from .. import sim

    local_gids = [
        int(cell.gid)
        for cell in sim.net.compartCells
    ]

    per_rank = sim.pc.py_allgather(
        local_gids
    )

    gids = []

    for rank_gids in per_rank:
        gids.extend(rank_gids)

    return sorted(set(gids))


def _write_report_conf(filename, gids):
    """
    Write the partially-binary report.conf expected by CoreNEURON.
    """

    from .. import sim

    report_dt = float(getattr(sim.cfg,'recordLFPStep',sim.cfg.recordStep)
)

    # Later we can add a separate cfg.recordLFPStep.
    start = 0.0
    stop = float(sim.cfg.duration)

    # Number used by SONATA reporting buffering.
    buffer_size = 8

    gid_array = array(
        'i',
        [int(g) for g in gids]
    )

    # CoreNEURON reads C int values here.
    if gid_array.itemsize != 4:
        raise RuntimeError(
            'Expected a 4-byte C int for CoreNEURON report GIDs'
        )

    with open(filename, 'wb') as f:

        # Number of reports
        f.write(b'1\n')

        # Parser order:
        #
        # name
        # target_name
        # report_type
        # report_on
        # units
        # format
        # sections
        # compartments
        # report_dt
        # start
        # stop
        # num_gids
        # buffer_size
        # scaling
        #
        metadata = (
            'netpyne_lfp.h5 '
            'All '
            'lfp '
            'i_membrane '
            'mV '
            'SONATA '
            'All '
            'All '
            f'{report_dt} '
            f'{start} '
            f'{stop} '
            f'{len(gids)} '
            f'{buffer_size} '
            'None\n'
        )

        f.write(
            metadata.encode('utf-8')
        )

        # Binary GID vector
        f.write(
            gid_array.tobytes()
        )

        f.write(b'\n')

        # Spike-report population metadata.
        #
        # CoreNEURON's parser expects this block even though
        # NetPyNE itself handles spikes separately.
        f.write(b'1\n')
        f.write(b'All 0\n')
        f.write(b'netpyne_core_spikes.h5\n')


def _write_sim_conf(
    filename,
    output_dir,
    report_conf
):
    with open(filename, 'w') as f:

        f.write(
            f"outpath='{output_dir}'\n"
        )

        f.write(
            f"report-conf='{report_conf}'\n"
        )

def setup_core_lfp():
    """
    Complete setup for native CoreNEURON LFP recording.
    """

    from .. import sim

    if not sim.cfg.coreneuron:
        return

    if not sim.cfg.recordLFP:
        return

    # ----------------------------------------------------------
    # 1. Register transfer-resistance factors
    # ----------------------------------------------------------

    register_core_lfp_mappings()

    # ----------------------------------------------------------
    # 2. Determine all biophysical GIDs
    # ----------------------------------------------------------

    gids = _get_all_compartment_gids()

    if len(gids) == 0:
        raise RuntimeError(
            '[CoreLFP] No compartment-cell GIDs found'
        )

    # ----------------------------------------------------------
    # 3. Output directory
    # ----------------------------------------------------------

    base_folder = getattr(
        sim.cfg,
        'saveFolder',
        '.'
    )

    output_dir = os.path.abspath(
        os.path.join(
            base_folder,
            'coreneuron_lfp'
        )
    )

    report_conf = os.path.join(
        output_dir,
        'report.conf'
    )

    sim_conf = os.path.join(
        output_dir,
        'sim.conf'
    )

    report_file = os.path.join(
        output_dir,
        'netpyne_lfp.h5'
    )

    # ----------------------------------------------------------
    # 4. Rank 0 creates configuration
    # ----------------------------------------------------------

    if sim.rank == 0:

        os.makedirs(
            output_dir,
            exist_ok=True
        )

        # Very important:
        # never accidentally load the previous simulation's report.
        if os.path.exists(report_file):
            os.remove(report_file)

        _write_report_conf(
            report_conf,
            gids
        )

        _write_sim_conf(
            sim_conf,
            output_dir,
            report_conf
        )

        print(
            '[CoreLFP] report.conf:',
            report_conf,
            flush=True
        )

        print(
            '[CoreLFP] expected report:',
            report_file,
            flush=True
        )

    sim.pc.barrier()

    # Same paths on every rank
    sim._coreLFP_output_dir = output_dir
    sim._coreLFP_report_conf = report_conf
    sim._coreLFP_sim_config = sim_conf
    sim._coreLFP_report_file = report_file

    if sim.rank == 0:
        print(
            f'[CoreLFP] Native CoreNEURON LFP ready for '
            f'{len(gids)} cells',
            flush=True
        )