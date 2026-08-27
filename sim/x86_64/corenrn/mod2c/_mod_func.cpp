#include <cstdio>
namespace coreneuron {
extern int nrnmpi_myid;
extern int nrn_nobanner_;
extern int
  _CaDynamics_reg(void),
  _Ca_HVA_reg(void),
  _Ca_LVA_reg(void),
  _Gfluct_reg(void),
  _Ih_reg(void),
  _Im_reg(void),
  _K_P_reg(void),
  _K_T_reg(void),
  _Kv3_1_reg(void),
  _NMDA_reg(void),
  _NaTg_reg(void),
  _Nap_reg(void),
  _ProbAMPANMDA_reg(void),
  _ProbUDFsyn_reg(void),
  _SK_reg(void),
  _epsp_reg(void),
  _exp2syn_reg(void),
  _expsyn_reg(void),
  _hh_reg(void),
  _netstim_reg(void),
  _passive_reg(void),
  _pattern_reg(void),
  _stim_reg(void),
  _svclmp_reg(void),
  _tonic_reg(void);

void modl_reg() {
    if (!nrn_nobanner_ && nrnmpi_myid < 1) {
        fprintf(stderr, " Additional mechanisms from files\n");
        fprintf(stderr, " CaDynamics.mod");
        fprintf(stderr, " Ca_HVA.mod");
        fprintf(stderr, " Ca_LVA.mod");
        fprintf(stderr, " Gfluct.mod");
        fprintf(stderr, " Ih.mod");
        fprintf(stderr, " Im.mod");
        fprintf(stderr, " K_P.mod");
        fprintf(stderr, " K_T.mod");
        fprintf(stderr, " Kv3_1.mod");
        fprintf(stderr, " NMDA.mod");
        fprintf(stderr, " NaTg.mod");
        fprintf(stderr, " Nap.mod");
        fprintf(stderr, " ProbAMPANMDA.mod");
        fprintf(stderr, " ProbUDFsyn.mod");
        fprintf(stderr, " SK.mod");
        fprintf(stderr, " epsp.mod");
        fprintf(stderr, " exp2syn.mod");
        fprintf(stderr, " expsyn.mod");
        fprintf(stderr, " hh.mod");
        fprintf(stderr, " netstim.mod");
        fprintf(stderr, " passive.mod");
        fprintf(stderr, " pattern.mod");
        fprintf(stderr, " stim.mod");
        fprintf(stderr, " svclmp.mod");
        fprintf(stderr, " tonic.mod");
        fprintf(stderr, "\n\n");
    }
    _CaDynamics_reg();
    _Ca_HVA_reg();
    _Ca_LVA_reg();
    _Gfluct_reg();
    _Ih_reg();
    _Im_reg();
    _K_P_reg();
    _K_T_reg();
    _Kv3_1_reg();
    _NMDA_reg();
    _NaTg_reg();
    _Nap_reg();
    _ProbAMPANMDA_reg();
    _ProbUDFsyn_reg();
    _SK_reg();
    _epsp_reg();
    _exp2syn_reg();
    _expsyn_reg();
    _hh_reg();
    _netstim_reg();
    _passive_reg();
    _pattern_reg();
    _stim_reg();
    _svclmp_reg();
    _tonic_reg();
}
} //namespace coreneuron
