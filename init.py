"""
init.py  ## see https://github.com/suny-downstate-medical-center/thalamus_netpyne/blob/main/sim/init.py for OU noise implementation 

Starting script to run NetPyNE-based 

Usage:
    python init.py # Run simulation, optionally plot a raster

MPI usage:
    mpiexec -n 8 nrniv -python -mpi init.py

"""

# import matplotlib; matplotlib.use('agg')  # to avoid graphics error in servers

from netpyne import sim
from neuron import h
import numpy as np
from mpi4py import MPI
from scipy import stats as st


from params.circuit_params import CELL_NAMES, SING_CELL_PARAM


# cfg, netParams = sim.readCmdLineArgs(simConfigDefault='cfg.py', netParamsDefault='netParams.py')
cfg, netParams = sim.readCmdLineArgs()
# sim.create(netParams, cfg)
# sim.createSimulateAnalyze(netParams, cfg)

#MPI variables:
COMM = MPI.COMM_WORLD
SIZE = COMM.Get_size()
RANK = COMM.Get_rank()
GLOBALSEED = int(cfg.GLOBALSEED)

# Create new RandomState for each RANK
SEED = GLOBALSEED*10000
np.random.seed(SEED + RANK)
local_state = np.random.RandomState(SEED + RANK)
halfnorm_rv = st.halfnorm
halfnorm_rv.random_state = local_state
uniform_rv = st.uniform
uniform_rv.random_state = local_state

sim.initialize(
    simConfig = cfg, 	
    netParams = netParams)  				# create network object and set cfg and net params
sim.net.createPops()               			# instantiate network populations
sim.net.createCells()              			# instantiate network cells based on defined populations

for metype in sim.net.cells:
    # print('cell ->', metype.tags['cellType'], metype.tags['cellModel'], metype.tags['pop'])
    if metype.tags['cellType'] == 'HL23PYR':
        metype.secs['axon_0']['hObj'](0.1).diam = 2.875
        metype.secs['axon_0']['hObj'](0.3).diam = 2.625
        metype.secs['axon_0']['hObj'](0.5).diam = 2.375
        metype.secs['axon_0']['hObj'](0.7).diam = 2.125
        metype.secs['axon_0']['hObj'](0.9).diam = 1.875

        metype.secs['axon_1']['hObj'](0.0714286).diam = 1.6964285714285714
        metype.secs['axon_1']['hObj'](0.214286).diam = 1.5892857142857142
        metype.secs['axon_1']['hObj'](0.357143).diam = 1.4821428571428572
        metype.secs['axon_1']['hObj'](0.5).diam = 1.375
        metype.secs['axon_1']['hObj'](0.642857).diam = 1.2678571428571428
        metype.secs['axon_1']['hObj'](0.785714).diam = 1.1607142857142856
        metype.secs['axon_1']['hObj'](0.928571).diam = 1.0535714285714286

    if metype.tags['cellType'] == 'HL23SST':
        metype.secs['axon_0']['hObj'](0.1).diam = 2.875
        metype.secs['axon_0']['hObj'](0.3).diam = 2.625
        metype.secs['axon_0']['hObj'](0.5).diam = 2.375
        metype.secs['axon_0']['hObj'](0.7).diam = 2.125
        metype.secs['axon_0']['hObj'](0.9).diam = 1.875

        metype.secs['axon_1']['hObj'](0.0714286).diam = 1.6964285714285714
        metype.secs['axon_1']['hObj'](0.214286).diam = 1.5892857142857142
        metype.secs['axon_1']['hObj'](0.357143).diam = 1.4821428571428572
        metype.secs['axon_1']['hObj'](0.5).diam = 1.375
        metype.secs['axon_1']['hObj'](0.642857).diam = 1.2678571428571428
        metype.secs['axon_1']['hObj'](0.785714).diam = 1.1607142857142856
        metype.secs['axon_1']['hObj'](0.928571).diam = 1.0535714285714286


# Also load net_functions.hoc so HOC helpers are available at cell-build time.
h.load_file('net_functions.hoc')

# Keep point-process objects alive for the full simulation.
_OU_REFS = []

# Distinct namespace for Gfluct2 Random123 streams.
_OU_STREAM_TAG = 0x4F550000
_UINT32_MASK = 0xFFFFFFFF


def _neuron_distance_setup(hobj):
    hobj.push()
    h.distance(0, 0.0)
    h.pop_section()


def _neuron_distance(hobj, x):
    hobj.push()
    d = h.distance(x)
    h.pop_section()
    return d


def _get_longest_branch(hobjs):
    max_L = 0.0
    for hobj in hobjs:
        sref = h.SectionRef(sec=hobj)
        if sref.nchild() == 0:
            d = _neuron_distance(hobj, 1)
            if d > max_L:
                max_L = d
    if max_L == 0.0 and hobjs:
        max_L = _neuron_distance(hobjs[0], 1)
    return max_L


def _locate_sites(hobjs, site):
    hits = []
    for hobj in hobjs:
        d0 = _neuron_distance(hobj, 0)
        d1 = _neuron_distance(hobj, 1)
        if d0 > d1:
            d0, d1 = d1, d0
        if d1 <= d0:
            continue
        if d0 <= site <= d1:
            x = (site - d0) / (d1 - d0)
            x = max(0.01, min(0.99, x))
            hits.append((hobj, x))
    return hits


def insert_ou_noise(sim_obj, cfg_obj, sing_cell_param):
    """Insert CoreNEURON 9.0.1-compatible OU conductance noise."""
    n_total = 0
    type_basal = {}
    type_apical = {}
    type_n_cells = {}

    global_seed = int(cfg_obj.GLOBALSEED) & _UINT32_MASK

    for cell in sim_obj.net.cells:
        cell_type = cell.tags.get("cellType", "")
        gou = sing_cell_param[cell_type]["GOU"]
        gid_seed = int(cell.gid) & _UINT32_MASK

        idx = 0
        n_basal = 0
        n_apical = 0

        def _place_ou(hobj, x_pos, relpos_for_g, site_id):
            g_val = gou * np.exp(relpos_for_g)
            ou = h.Gfluct2(x_pos, sec=hobj)

            ou.E_e = 0.0
            ou.E_i = -80.0
            ou.g_e0 = g_val
            ou.g_i0 = 0.0
            ou.std_e = g_val
            ou.std_i = 0.0
            ou.tau_e = 65.0
            ou.tau_i = 20.0

            # In NEURON 9.0.1, set RANDOM stream properties through the
            # NMODLRandom wrapper instead of random_setids() in the MOD file.
            stream_id3 = (_OU_STREAM_TAG + int(site_id)) & _UINT32_MASK
            ou.rng.set_ids(global_seed, gid_seed, stream_id3)
            ou.rng.set_seq(0)

            _OU_REFS.append(ou)

        dend_secs = [name for name in cell.secs if name.startswith("dend")]
        if dend_secs:
            dend_hobjs = [cell.secs[name]["hObj"] for name in dend_secs]
            _neuron_distance_setup(dend_hobjs[0])
            max_L = _get_longest_branch(dend_hobjs)

            for basal_index, (hobj, x) in enumerate(
                _locate_sites(dend_hobjs, 0.5 * max_L)
            ):
                if cell.gid >= 0:
                    _place_ou(
                        hobj,
                        x,
                        relpos_for_g=0.5,
                        site_id=5 + basal_index,
                    )
                idx += 1
                n_basal += 1

        if "PYR" in cell_type:
            apic_secs = [name for name in cell.secs if name.startswith("apic")]
            if apic_secs:
                apic_hobjs = [cell.secs[name]["hObj"] for name in apic_secs]
                _neuron_distance_setup(apic_hobjs[0])
                max_L_apic = _get_longest_branch(apic_hobjs)

                for apical_index, relpos in enumerate((0.1, 0.3, 0.5, 0.7, 0.9)):
                    hits = _locate_sites(apic_hobjs, relpos * max_L_apic)
                    if not hits:
                        hits = [(apic_hobjs[0], min(relpos, 0.9))]

                    best_hobj, best_x = max(
                        hits,
                        key=lambda item: item[0](item[1]).diam,
                    )
                    if cell.gid >= 0:
                        _place_ou(
                            best_hobj,
                            best_x,
                            relpos_for_g=relpos,
                            site_id=apical_index,
                        )
                    idx += 1
                    n_apical += 1

        n_total += idx
        type_basal[cell_type] = type_basal.get(cell_type, 0) + n_basal
        type_apical[cell_type] = type_apical.get(cell_type, 0) + n_apical
        type_n_cells[cell_type] = type_n_cells.get(cell_type, 0) + 1

    for cell_type in sorted(type_basal):
        n_cells = type_n_cells[cell_type]
        basal_per_cell = type_basal[cell_type] // n_cells
        apical_per_cell = type_apical.get(cell_type, 0) // n_cells
        g_basal_ps = sing_cell_param[cell_type]["GOU"] * np.exp(0.5) * 1e6
        # Optional diagnostics:
        # print(
        #     f"[OU] {cell_type}: {basal_per_cell} basal/cell, "
        #     f"{apical_per_cell} apical/cell, "
        #     f"g_e0_basal={g_basal_ps:.1f} pS"
        # )

    return n_total


# ---------------------------------------------------------------------------
# 3.  Helper: insert tonic GABA inhibition per cell
# ---------------------------------------------------------------------------
def insert_tonic_gaba(sim_obj, cfg_obj, sing_cell_param):
    """Insert tonic GABA (tonic.mod) into soma/basal of all cells,
    and into apical of PYR cells."""
    for cell in sim_obj.net.cells:
        if cell.gid >= 0: # compare with LFPy
            cell_type = cell.tags.get('cellType', '')
            p = sing_cell_param[cell_type]

            if cfg_obj.DRUG:
                g_soma = p['drug_tonic']
                g_apic = p['drug_apic_tonic']
            else:
                g_soma = p['norm_tonic']
                g_apic = p['apic_tonic']

            for sname in [s for s in cell.secs if s.startswith('soma')]:
                sec = cell.secs[sname]['hObj']
                sec.insert('tonic')
                for seg in sec:
                    seg.tonic.g      = g_soma
                    seg.tonic.e_gaba = -75.0

            for sname in [s for s in cell.secs if s.startswith('dend')]:
                sec = cell.secs[sname]['hObj']
                sec.insert('tonic')
                for seg in sec:
                    seg.tonic.g      = g_soma
                    seg.tonic.e_gaba = -75.0

            if 'PYR' in cell_type:
                for sname in [s for s in cell.secs if s.startswith('apic')]:
                    sec = cell.secs[sname]['hObj']
                    sec.insert('tonic')
                    for seg in sec:
                        seg.tonic.g      = g_apic
                        seg.tonic.e_gaba = -75.0

    # print(f'[init] Inserted tonic GABA into {len(sim_obj.net.cells)} cells.')

# ---------------------------------------------------------------------------
# 5.  Build and run simulation
# ---------------------------------------------------------------------------

#print('[init] Inserting background noise (Gfluct2) ...')
insert_ou_noise(sim, cfg, SING_CELL_PARAM)

# print('[init] Inserting tonic GABA inhibition ...')
insert_tonic_gaba(sim, cfg, SING_CELL_PARAM)

sim.cfg.distributeSynsUniformly = False

# print('[init] Creating connections ...')
sim.net.connectCells()

# print('[init] Adding external stimuli ...')
sim.net.addStims()

# print('[init] Setting up recording ...')
sim.setupRecording()

# print('[init] Running simulation ...')
sim.runSim()

# print('[init] Gathering data ...')
sim.gatherData()

# print('[init] Saving data ...')
sim.saveData()

# print('[init] Plotting data ...')
sim.analysis.plotData()           		# plot spikes, V traces, rasters, etc.