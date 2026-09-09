# =============================================================================
# cfg.py  —  SimConfig for L23Net NetPyNE Replica (Yao et al. 2022)
# =============================================================================
import sys
import pandas as pd
from netpyne import specs

cfg = specs.SimConfig()
cfg.coreneuron = True
cfg.gpu = True
# ---------------------------------------------------------------------------
# Identity / I/O
# ---------------------------------------------------------------------------
cfg.simLabel = 'v3_data'       #   + str(cfg.cynradNumber)
cfg.saveFolder = '../data/'+cfg.simLabel
cfg.savePickle  = True
cfg.saveJson    = False   # disabled for speed — spikes saved as .npy per seed

cfg.saveDataInclude = ['simData' , 'simConfig', 'netParams']
cfg.backupCfgFile = None 		##  
cfg.gatherOnlySimData = False	##  
cfg.saveCellSecs = True			##  
cfg.saveCellConns = True		##  

cfg.verbose = False
cfg.hParams = {'celsius': 34, 'v_init': -80}  
cfg.verbose = False
cfg.createNEURONObj = True
cfg.createPyStruct = True
cfg.cvode_active = False
cfg.cvode_atol = 1e-6
cfg.cache_efficient = True
cfg.printRunTime = 0.5

cfg.includeParamsLabel = False
cfg.printPopAvgRates = True
cfg.checkErrors = False

cfg.saveValidationConns = False ########## when this setting is True the model generate a new random network using the globalSEED, in this case set cfg.loadValidationConns = False
cfg.loadValidationConns = True

cfg.validationConnFile = 'Connections_rules.pkl'

# ---------------------------------------------------------------------------
# Simulation timing
# ---------------------------------------------------------------------------
cfg.duration = 2000.0    # ms
cfg.dt       = 0.025     # ms  (40 kHz)

# ---------------------------------------------------------------------------
# Random seeds
# ---------------------------------------------------------------------------
cfg.seeds = {'conn':  54321, 'stim':  54321, 'loc':   54321, 'cell':  54321}
#cfg.GLOBALSEED = 54321
cfg.DRUG = False

# ---------------------------------------------------------------------------
# LFP recording electrode (only used when cfg.rec_LFP = True)
# ---------------------------------------------------------------------------
cfg.rec_LFP = False
if cfg.rec_LFP == True:
    cfg.recordLFPStep = 0.025 # ms
    cfg.recordLFP = [[0.0, 0.0, 5.0]]

# ---------------------------------------------------------------------------
# Recording — one soma trace per population
# GID layout (full run): PYR 0-799, SST 800-849, PV 850-919, VIP 920-999
# ---------------------------------------------------------------------------


circuit_params = pd.read_excel('Circuit_param.xls', sheet_name = None, index_col = 0)
N_HL23PYR = int(circuit_params['SING_CELL_PARAM'].at['cell_num','HL23PYR'])
N_HL23SST = int(circuit_params['SING_CELL_PARAM'].at['cell_num','HL23SST'])
N_HL23PV = int(circuit_params['SING_CELL_PARAM'].at['cell_num','HL23PV'])
N_HL23VIP = int(circuit_params['SING_CELL_PARAM'].at['cell_num','HL23VIP'])

N_cells = N_HL23PYR + N_HL23SST + N_HL23PV + N_HL23VIP

cell_indices_to_plot = [0, 1, 
                        N_HL23PYR, N_HL23PYR + 1, 
                        N_HL23PYR + N_HL23SST, N_HL23PYR + N_HL23SST + 1, 
                        N_HL23PYR + N_HL23SST + N_HL23PV, N_HL23PYR + N_HL23SST + N_HL23PV + 1]

cfg.recordCells  = cell_indices_to_plot
cfg.recordTraces = {'V_soma': {'sec': 'soma_0', 'loc': 0.5, 'var': 'v'}}

cfg.cellNumber = {}
cfg.cellNumber['HL23PYR'] = N_HL23PYR
cfg.cellNumber['HL23SST'] = N_HL23SST
cfg.cellNumber['HL23PV'] = N_HL23PV
cfg.cellNumber['HL23VIP'] = N_HL23VIP

cfg.cellNumber0 = {}
cfg.cellNumber0['HL23PYR'] = 0
cfg.cellNumber0['HL23SST'] = N_HL23PYR
cfg.cellNumber0['HL23PV'] = N_HL23PYR + N_HL23SST
cfg.cellNumber0['HL23VIP'] = N_HL23PYR + N_HL23SST + N_HL23PV


cfg.cellTypeByIndex = {}

for i in range(N_HL23PYR):
    cfg.cellTypeByIndex[i] = 'HL23PYR'

for i in range(N_HL23PYR, N_HL23PYR + N_HL23SST):
    cfg.cellTypeByIndex[i] = 'HL23SST'

for i in range(N_HL23PYR + N_HL23SST,
               N_HL23PYR + N_HL23SST + N_HL23PV):
    cfg.cellTypeByIndex[i] = 'HL23PV'

for i in range(N_HL23PYR + N_HL23SST + N_HL23PV, N_cells):
    cfg.cellTypeByIndex[i] = 'HL23VIP'


# LFPy conn distribution comparation
cfg.gExc = 0.945 # The factor ~0.945 compensates for the main conn difference
cfg.ROTATE_Y = True
cfg.Change_axon_names = True

#------------------------------------------------------------------------------
# Analysis and plotting 
# ------------------------------------------------------------------------------

Ipops = ['HL23SST', 'HL23PV', 'HL23VIP']
Epops = ['HL23PYR']

cfg.allpops = Epops + Ipops

cfg.recordStep = cfg.dt    # record every time step

cfg.analysis['plotRaster'] = {'include':  cfg.allpops, 'saveFig': True, 'showFig': False,'orderInverse': True, 'timeRange': [0,cfg.duration], 'figSize': (12,6), 'fontSize':4, 'markerSize':4, 'marker': 'o', 'dpi': 300} 
cfg.analysis['plot2Dnet']   = {'include':  cfg.allpops, 'saveFig': True, 'showConns': False, 'figSize': (15,15), 'view': 'xy', 'fontSize':16}   # Plot 2D cells xy
cfg.analysis['plot3Dnet']   = {'include':  cfg.allpops, 'saveFig': True, 'showConns': False, 'figSize': (15,15), 'view': 'xy', 'fontSize':16}   # Plot 2D cells xy
cfg.analysis['plotTraces'] = {'include': cfg.recordCells, 'oneFigPer': 'trace', 'axis': False, 'overlay': False, 'timeRange': [1000,cfg.duration], 'saveFig': True, 'subtitles': None, 'legend': None, 'showFig': False, 'figSize':(18,18)}
cfg.analysis['plotConn'] = {'includePre': cfg.allpops, 'includePost': cfg.allpops, 'feature': 'numConns', 'groupBy': 'pop', 'figSize': (24,24), 
                            'saveFig': True, 'orderBy': 'gid', 'graphType': 'matrix', 'saveData':'v1_batch1_matrix_numConn.json', 'fontSize': 18}


#------------------------------------------------------------------------------
# General network parameters
#------------------------------------------------------------------------------
cfg.scale = 1.0 # reduce size
cfg.sizeY = 3300.0
cfg.sizeX = 500.0 # r = 250 um 
cfg.sizeZ = 500.0

# ---------------------------------------------------------------------------
# Validation IClamp
# ---------------------------------------------------------------------------
cfg.IClamp_val_act = False

if cfg.IClamp_val_act:
    cfg.IClamp_delay = 20.0   # ms
    cfg.IClamp_dur   = 5000.0   # ms

    cfg.IClamp_amp = {
        'HL23PYR': 0.1,
        'HL23SST': 0.1,
        'HL23PV':  0.1,
        'HL23VIP': 0.1,
    }