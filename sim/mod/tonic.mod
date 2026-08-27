TITLE Tonic GABA current with rectification

COMMENT
    Tonic GABA current with voltage-dependent rectification.

    Based on Pavlov et al. (J Neurosci. 2009).

    Original kinetic scheme:

        o <-> c

    with:

        b = forward rate constant
        a = backward rate constant

    Conservation:

        o + c = 1

    The system is represented using a single independent
    state variable o. The closed-state probability is:

        c = 1 - o
ENDCOMMENT


NEURON {
    SUFFIX tonic

    NONSPECIFIC_CURRENT i

    RANGE g
    RANGE e_gaba
    RANGE i
    RANGE o
    RANGE c
    RANGE a
    RANGE b
    THREADSAFE
}


UNITS {
    (S) = (siemens)
    (mV) = (millivolt)
    (mA) = (milliamp)
}


PARAMETER {
    g = 0.001 (S/cm2)
    e_gaba = -80 (mV)
}


ASSIGNED {
    v       (mV)
    i       (mA/cm2)

    a       (/ms)
    b       (/ms)

    c
}


STATE {
    o
}


BREAKPOINT {
    SOLVE states METHOD cnexp

    c = 1 - o

    i = g * o * (v - e_gaba)
}


DERIVATIVE states {
    rates()

    o' = a * (1 - o) - b * o
}


INITIAL {
    rates()
    o = a/(a+b)
    c = 1-o
}


PROCEDURE rates() {
    LOCAL x, y

    UNITSOFF

    x = 0.1 * (v + 20)

    if (fabs(x) > 1e-6) {
        a = (50 * x) / (1 - exp(-x))
    } else {
        a = 0.25 * (v^2 + 20 * v + 200)
    }

    y = -0.08 * (v - 10)

    if (fabs(y) > 1e-6) {
        b = (20 * y) / (1 - exp(-y))
    } else {
        b = -0.064 * (v^2 - 45 * v + 37.5)
    }

    UNITSON
}