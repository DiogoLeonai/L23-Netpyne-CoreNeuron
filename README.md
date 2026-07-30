# Human Cortex Layer 2/3 Model with NetPyNE and CoreNEURON GPU Acceleration

This repository contains an implementation of a **human cortical layer 2/3 (L2/3) network model** developed using **NetPyNE** and adapted to run with **CoreNEURON**, enabling accelerated simulations using GPUs.

The model is based on a detailed biophysical representation of human cortical L2/3 neurons, including morphologically detailed cells, synaptic mechanisms, and network connectivity. The implementation uses the NetPyNE framework for network construction and simulation setup, while CoreNEURON is used as the simulation engine to enable high-performance execution and GPU acceleration.

## Features

* Biophysically detailed model of the **human cerebral cortex, Layer 2/3**.
* Implemented using **NetPyNE**.
* Adapted to run with **CoreNEURON**.
* Supports **GPU-accelerated simulations** using CUDA-compatible NVIDIA GPUs.
* Uses morphologically detailed neurons and biophysical mechanisms.
* Supports recording of neuronal membrane voltage traces.
* Compatible with CoreNEURON-compiled NMODL mechanisms.

## CoreNEURON and GPU Acceleration

CoreNEURON provides an optimized simulation engine designed for large-scale neuronal network simulations. When configured with GPU support, the model can take advantage of NVIDIA GPUs to accelerate the simulation of computationally demanding networks.

The model mechanisms must be compiled with CoreNEURON support. For example, the NMODL mechanisms can be compiled using the appropriate CoreNEURON compilation option:

```bash
nrnivmodl -coreneuron mod
```

The exact compilation and execution commands may depend on the installed versions of NEURON, CoreNEURON, NetPyNE, CUDA, and the available GPU hardware.

## Important: Voltage Trace Recording with CoreNEURON

During the adaptation of the model to CoreNEURON, an issue was identified with the recording of voltage traces through NetPyNE.

When using NetPyNE's standard `recordTraces()` implementation, voltage recordings are created using `h.Vector.record()` with an explicit `recordStep` argument:

```python
vector.record(ptr, recordStep)
```

When running with CoreNEURON, this caused the recorded voltage vectors to contain only the initial membrane potential instead of the complete voltage trace.

A minimal CoreNEURON test showed that the following worked correctly:

```python
vector.record(ptr)
```

while:

```python
vector.record(ptr, recordStep)
```

did not correctly populate the recording vector.

The issue was therefore addressed by modifying the `recordTraces()` function in NetPyNE's `cell.py`.

For CoreNEURON simulations, the recording call was changed to omit the explicit `recordStep`:

```python
if sim.cfg.coreneuron:
    vector.record(ptr)
else:
    vector.record(ptr, recordStep)
```

The same modification was applied to the branches handling single and multiple recording pointers.

For example:

```python
if sim.cfg.coreneuron:
    sim.simData[key]['cell_' + str(self.gid)].record(ptr)
else:
    sim.simData[key]['cell_' + str(self.gid)].record(
        ptr, sim.cfg.recordStep
    )
```

and for multiple pointers:

```python
if sim.cfg.coreneuron:
    sim.simData[key]['cell_' + str(self.gid)][secLoc].record(ptrItem)
else:
    sim.simData[key]['cell_' + str(self.gid)][secLoc].record(
        ptrItem, recordStep
    )
```

This modification allowed the full membrane voltage traces to be correctly recorded during CoreNEURON simulations.

### Important Note

This modification is currently a workaround for the CoreNEURON recording compatibility issue. Because `recordStep` is omitted from `Vector.record()` when CoreNEURON is enabled, the recording interval may correspond to the simulation timestep (`dt`) rather than the NetPyNE `recordStep` parameter.

If a lower sampling frequency is required, the recorded voltage traces can be downsampled after the simulation.

## Software Environment

The implementation was tested with:

* Python 3.10
* NEURON 9.0.1
* NetPyNE 1.1.1
* CoreNEURON
* CUDA-compatible NVIDIA GPU

The exact software versions and hardware requirements may vary depending on the desired simulation configuration.

## Repository Structure

The repository contains the scripts and configuration files required to construct and run the human cortical L2/3 network model, as well as the adaptations required for CoreNEURON execution and GPU acceleration.

The NetPyNE source code modification described above is required for voltage trace recording when using CoreNEURON.

## Usage

The general workflow for running the model is:

1. Install NEURON and NetPyNE.
2. Install and configure CoreNEURON with GPU support.
3. Compile the model's NMODL mechanisms with CoreNEURON support.
4. Apply the `cell.py` modification described above to enable voltage trace recording.
5. Configure the simulation to use CoreNEURON and GPU acceleration.
6. Run the NetPyNE simulation.

The exact commands and configuration options depend on the local environment and GPU configuration.

The repository provides two versions of the simulation code:

Python script (.py): The main script-based version for running the model directly from the command line or in batch simulations.
Jupyter Notebook (.ipynb): An interactive version of the model, useful for step-by-step execution, testing, debugging, and inspecting simulation results.

Both versions implement the same human cortical Layer 2/3 model using NetPyNE and CoreNEURON, with support for GPU acceleration.
