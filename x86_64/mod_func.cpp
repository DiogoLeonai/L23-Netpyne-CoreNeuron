#include <stdio.h>
#include "hocdec.h"
extern int nrnmpi_myid;
extern int nrn_nobanner_;
#if defined(__cplusplus)
extern "C" {
#endif

extern void _CaDynamics_reg(void);
extern void _Ca_HVA_reg(void);
extern void _Ca_LVA_reg(void);
extern void _epsp_reg(void);
extern void _Gfluct_reg(void);
extern void _Ih_reg(void);
extern void _Im_reg(void);
extern void _K_P_reg(void);
extern void _K_T_reg(void);
extern void _Kv3_1_reg(void);
extern void _Nap_reg(void);
extern void _NaTg_reg(void);
extern void _NMDA_reg(void);
extern void _ProbAMPANMDA_reg(void);
extern void _ProbUDFsyn_reg(void);
extern void _SK_reg(void);
extern void _tonic_reg(void);

void modl_reg() {
  if (!nrn_nobanner_) if (nrnmpi_myid < 1) {
    fprintf(stderr, "Additional mechanisms from files\n");
    fprintf(stderr, " \"mod/CaDynamics.mod\"");
    fprintf(stderr, " \"mod/Ca_HVA.mod\"");
    fprintf(stderr, " \"mod/Ca_LVA.mod\"");
    fprintf(stderr, " \"mod/epsp.mod\"");
    fprintf(stderr, " \"mod/Gfluct.mod\"");
    fprintf(stderr, " \"mod/Ih.mod\"");
    fprintf(stderr, " \"mod/Im.mod\"");
    fprintf(stderr, " \"mod/K_P.mod\"");
    fprintf(stderr, " \"mod/K_T.mod\"");
    fprintf(stderr, " \"mod/Kv3_1.mod\"");
    fprintf(stderr, " \"mod/Nap.mod\"");
    fprintf(stderr, " \"mod/NaTg.mod\"");
    fprintf(stderr, " \"mod/NMDA.mod\"");
    fprintf(stderr, " \"mod/ProbAMPANMDA.mod\"");
    fprintf(stderr, " \"mod/ProbUDFsyn.mod\"");
    fprintf(stderr, " \"mod/SK.mod\"");
    fprintf(stderr, " \"mod/tonic.mod\"");
    fprintf(stderr, "\n");
  }
  _CaDynamics_reg();
  _Ca_HVA_reg();
  _Ca_LVA_reg();
  _epsp_reg();
  _Gfluct_reg();
  _Ih_reg();
  _Im_reg();
  _K_P_reg();
  _K_T_reg();
  _Kv3_1_reg();
  _Nap_reg();
  _NaTg_reg();
  _NMDA_reg();
  _ProbAMPANMDA_reg();
  _ProbUDFsyn_reg();
  _SK_reg();
  _tonic_reg();
}

#if defined(__cplusplus)
}
#endif
