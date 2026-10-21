#!/usr/bin/env python

# BraggPeakGSI_MT portal parser: metadata() reads the macro, parse() the
# captured output_0.out depth-dose table.
#
# Ported from geant-config-generator/tests/geant4/BraggPeakGSI_MT/parser.py
# (gts.BaseParser) to the module-level metadata()/parse() API expected by
# ci-workflows/validation/export.py.
import os

from geantval import getJSON, one_command, single_run


def metadata(commands):
    single_run(commands)
    beam_energy = one_command(commands, "/beam/energy/mean").split()[0]
    ion = one_command(commands, "/beam/ion", default=None)
    beam = "C12" if ion is not None and ion.split()[:2] == ["6", "12"] else ion
    return {"TEST": "BraggPeakGSI_MT",
            "PHYSLIST": one_command(commands, "/physics/referencePL"),
            "BEAM": beam,
            "BEAM_ENERGY_MEV": float(beam_energy)}


def read_output(filename):
    x, y, yerr = [], [], []
    with open(filename) as f:
        for line in f:
            line = line.strip()
            if line.startswith("#"):
                continue
            if not line:
                break
            fields = [float(v) for v in line.split()]
            y.append(fields[1])
            yerr.append(fields[2] * fields[1])
            x.append(fields[5])
    if not x:
        raise ValueError("No data rows found in " + filename)
    return x, y, yerr


def parse(job):
    x, y, yerr = read_output(os.path.join(job["path"], "output_0.out"))
    rjson = getJSON(job, "chart",
                     mctool_name="GEANT4",
                     mctool_model=job["PHYSLIST"],
                     observableName="energy deposition",
                     targetName="water",
                     beamParticle=job["BEAM"],
                     beamEnergies=[job["BEAM_ENERGY_MEV"]],
                     title="E_{dep} in water",
                     xAxisName="Depth in water, mm",
                     yAxisName="E_{dep}, a.u.",
                     xValues=x,
                     yValues=y,
                     yStatErrorsPlus=yerr,
                     yStatErrorsMinus=yerr)
    yield rjson
