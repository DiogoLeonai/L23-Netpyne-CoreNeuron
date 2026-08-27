/* Created by Language version: 7.7.0 */
/* VECTORIZED */
#define NRN_VECTORIZED 1
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mech_api.h"
#undef PI
#define nil 0
#define _pval pval
// clang-format off
#include "md1redef.h"
#include "section_fwd.hpp"
#include "nrniv_mf.h"
#include "md2redef.h"
#include "nrnconf.h"
// clang-format on
#include "neuron/cache/mechanism_range.hpp"
static constexpr auto number_of_datum_variables = 3;
static constexpr auto number_of_floating_point_variables = 21;
namespace {
template <typename T>
using _nrn_mechanism_std_vector = std::vector<T>;
using _nrn_model_sorted_token = neuron::model_sorted_token;
using _nrn_mechanism_cache_range = neuron::cache::MechanismRange<number_of_floating_point_variables, number_of_datum_variables>;
using _nrn_mechanism_cache_instance = neuron::cache::MechanismInstance<number_of_floating_point_variables, number_of_datum_variables>;
using _nrn_non_owning_id_without_container = neuron::container::non_owning_identifier_without_container;
template <typename T>
using _nrn_mechanism_field = neuron::mechanism::field<T>;
template <typename... Args>
void _nrn_mechanism_register_data_fields(Args&&... args) {
  neuron::mechanism::register_data_fields(std::forward<Args>(args)...);
}
}
 
#if !NRNGPU
#undef exp
#define exp hoc_Exp
#if NRN_ENABLE_ARCH_INDEP_EXP_POW
#undef pow
#define pow hoc_pow
#endif
#endif
 
#define nrn_init _nrn_init__Gfluct2
#define _nrn_initial _nrn_initial__Gfluct2
#define nrn_cur _nrn_cur__Gfluct2
#define _nrn_current _nrn_current__Gfluct2
#define nrn_jacob _nrn_jacob__Gfluct2
#define nrn_state _nrn_state__Gfluct2
#define _net_receive _net_receive__Gfluct2 
#define oup oup__Gfluct2 
 
#define _threadargscomma_ _ml, _iml, _ppvar, _thread, _globals, _nt,
#define _threadargsprotocomma_ Memb_list* _ml, size_t _iml, Datum* _ppvar, Datum* _thread, double* _globals, NrnThread* _nt,
#define _internalthreadargsprotocomma_ _nrn_mechanism_cache_range* _ml, size_t _iml, Datum* _ppvar, Datum* _thread, double* _globals, NrnThread* _nt,
#define _threadargs_ _ml, _iml, _ppvar, _thread, _globals, _nt
#define _threadargsproto_ Memb_list* _ml, size_t _iml, Datum* _ppvar, Datum* _thread, double* _globals, NrnThread* _nt
#define _internalthreadargsproto_ _nrn_mechanism_cache_range* _ml, size_t _iml, Datum* _ppvar, Datum* _thread, double* _globals, NrnThread* _nt
 	/*SUPPRESS 761*/
	/*SUPPRESS 762*/
	/*SUPPRESS 763*/
	/*SUPPRESS 765*/
	 extern double *hoc_getarg(int);
 
#define t _nt->_t
#define dt _nt->_dt
#define E_e _ml->template fpfield<0>(_iml)
#define E_e_columnindex 0
#define E_i _ml->template fpfield<1>(_iml)
#define E_i_columnindex 1
#define g_e0 _ml->template fpfield<2>(_iml)
#define g_e0_columnindex 2
#define g_i0 _ml->template fpfield<3>(_iml)
#define g_i0_columnindex 3
#define std_e _ml->template fpfield<4>(_iml)
#define std_e_columnindex 4
#define std_i _ml->template fpfield<5>(_iml)
#define std_i_columnindex 5
#define tau_e _ml->template fpfield<6>(_iml)
#define tau_e_columnindex 6
#define tau_i _ml->template fpfield<7>(_iml)
#define tau_i_columnindex 7
#define i _ml->template fpfield<8>(_iml)
#define i_columnindex 8
#define g_e _ml->template fpfield<9>(_iml)
#define g_e_columnindex 9
#define g_i _ml->template fpfield<10>(_iml)
#define g_i_columnindex 10
#define g_e1 _ml->template fpfield<11>(_iml)
#define g_e1_columnindex 11
#define g_i1 _ml->template fpfield<12>(_iml)
#define g_i1_columnindex 12
#define D_e _ml->template fpfield<13>(_iml)
#define D_e_columnindex 13
#define D_i _ml->template fpfield<14>(_iml)
#define D_i_columnindex 14
#define exp_e _ml->template fpfield<15>(_iml)
#define exp_e_columnindex 15
#define exp_i _ml->template fpfield<16>(_iml)
#define exp_i_columnindex 16
#define amp_e _ml->template fpfield<17>(_iml)
#define amp_e_columnindex 17
#define amp_i _ml->template fpfield<18>(_iml)
#define amp_i_columnindex 18
#define v _ml->template fpfield<19>(_iml)
#define v_columnindex 19
#define _g _ml->template fpfield<20>(_iml)
#define _g_columnindex 20
#define _nd_area *_ml->dptr_field<0>(_iml)
 
 //RANDOM variables 
 #define rng	(nrnran123_State*)_ppvar[2].get<void*>()
 #define _p_rng _ppvar[2].literal_value<void*>()
 
 /* Thread safe. No static _ml, _iml or _ppvar. */
 static int hoc_nrnpointerindex =  -1;
 static _nrn_mechanism_std_vector<Datum> _extcall_thread;
 /* external NEURON variables */
 /* declaration of user functions */
 static double _hoc_oup(void*);
 static int _mechtype;
extern void _nrn_cacheloop_reg(int, int);
extern void hoc_register_limits(int, HocParmLimits*);
extern void hoc_register_units(int, HocParmUnits*);
extern void nrn_promote(Prop*, int, int);
 
#define NMODL_TEXT 1
#if NMODL_TEXT
static void register_nmodl_text_and_filename(int mechtype);
#endif
 extern Prop* nrn_point_prop_;
 static int _pointtype;
 static void* _hoc_create_pnt(Object* _ho) { void* create_point_process(int, Object*);
 return create_point_process(_pointtype, _ho);
}
 static void _hoc_destroy_pnt(void*);
 static double _hoc_loc_pnt(void* _vptr) {double loc_point_process(int, void*);
 return loc_point_process(_pointtype, _vptr);
}
 static double _hoc_has_loc(void* _vptr) {double has_loc_point(void*);
 return has_loc_point(_vptr);
}
 static double _hoc_get_loc_pnt(void* _vptr) {
 double get_loc_point_process(void*); return (get_loc_point_process(_vptr));
}
 static void _hoc_setdata(void*);
 /* connect user functions to hoc names */
 static VoidFunc hoc_intfunc[] = {
 {0, 0}
};
 static Member_func _member_func[] = {
 {"loc", _hoc_loc_pnt},
 {"has_loc", _hoc_has_loc},
 {"get_loc", _hoc_get_loc_pnt},
 {"oup", _hoc_oup},
 {0, 0}
};
 /* declare global and static user variables */
 #define gind 0
 #define _gth 0
 /* some parameters have upper and lower limits */
 static HocParmLimits _hoc_parm_limits[] = {
 {0, 0, 0}
};
 static HocParmUnits _hoc_parm_units[] = {
 {"E_e", "mV"},
 {"E_i", "mV"},
 {"g_e0", "umho"},
 {"g_i0", "umho"},
 {"std_e", "umho"},
 {"std_i", "umho"},
 {"tau_e", "ms"},
 {"tau_i", "ms"},
 {"i", "nA"},
 {"g_e", "umho"},
 {"g_i", "umho"},
 {"g_e1", "umho"},
 {"g_i1", "umho"},
 {"D_e", "umho umho/ms"},
 {"D_i", "umho umho/ms"},
 {0, 0}
};
 static double delta_t = 0.01;
 /* connect global user variables to hoc */
 static DoubScal hoc_scdoub[] = {
 {0, 0}
};
 static DoubVec hoc_vdoub[] = {
 {0, 0, 0}
};
 static double _sav_indep;
 extern void _nrn_setdata_reg(int, void(*)(Prop*));
 static void _setdata(Prop* _prop) {
 }
 static void _hoc_setdata(void* _vptr) { Prop* _prop;
 _prop = ((Point_process*)_vptr)->_prop;
   _setdata(_prop);
 }
 static void nrn_alloc(Prop*);
static void nrn_init(_nrn_model_sorted_token const&, NrnThread*, Memb_list*, int);
static void nrn_state(_nrn_model_sorted_token const&, NrnThread*, Memb_list*, int);
 static void nrn_cur(_nrn_model_sorted_token const&, NrnThread*, Memb_list*, int);
static void nrn_jacob(_nrn_model_sorted_token const&, NrnThread*, Memb_list*, int);
 static void _hoc_destroy_pnt(void* _vptr) {
   destroy_point_process(_vptr);
}
 
static int _ode_count(int);
 /* connect range variables in _p that hoc is supposed to know about */
 static const char *_mechanism[] = {
 "7.7.0",
"Gfluct2",
 "E_e",
 "E_i",
 "g_e0",
 "g_i0",
 "std_e",
 "std_i",
 "tau_e",
 "tau_i",
 0,
 "i",
 "g_e",
 "g_i",
 "g_e1",
 "g_i1",
 "D_e",
 "D_i",
 0,
 0,
 0};
 
 /* Used by NrnProperty */
 static _nrn_mechanism_std_vector<double> _parm_default{
     0, /* E_e */
     -75, /* E_i */
     0.0121, /* g_e0 */
     0.0573, /* g_i0 */
     0.003, /* std_e */
     0.0066, /* std_i */
     2.728, /* tau_e */
     10.49, /* tau_i */
 }; 
 
static void _mech_inst_destruct(Prop* _prop);
 
 
extern Prop* need_memb(Symbol*);
static void nrn_alloc(Prop* _prop) {
  Prop *prop_ion{};
  Datum *_ppvar{};
  if (nrn_point_prop_) {
    _nrn_mechanism_access_alloc_seq(_prop) = _nrn_mechanism_access_alloc_seq(nrn_point_prop_);
    _ppvar = _nrn_mechanism_access_dparam(nrn_point_prop_);
  } else {
   _ppvar = nrn_prop_datum_alloc(_mechtype, 3, _prop);
    _nrn_mechanism_access_dparam(_prop) = _ppvar;
     _nrn_mechanism_cache_instance _ml_real{_prop};
    auto* const _ml = &_ml_real;
    size_t const _iml{};
    assert(_nrn_mechanism_get_num_vars(_prop) == 21);
 	/*initialize range parameters*/
 	E_e = _parm_default[0]; /* 0 */
 	E_i = _parm_default[1]; /* -75 */
 	g_e0 = _parm_default[2]; /* 0.0121 */
 	g_i0 = _parm_default[3]; /* 0.0573 */
 	std_e = _parm_default[4]; /* 0.003 */
 	std_i = _parm_default[5]; /* 0.0066 */
 	tau_e = _parm_default[6]; /* 2.728 */
 	tau_i = _parm_default[7]; /* 10.49 */
  }
 	 assert(_nrn_mechanism_get_num_vars(_prop) == 21);
 	_nrn_mechanism_access_dparam(_prop) = _ppvar;
 	/*connect ionic variables to this model*/
 _p_rng = (void*)nrnran123_newstream();
 nrn_mech_inst_destruct[_mechtype] = _mech_inst_destruct;
 
}
 static void _initlists();
 extern Symbol* hoc_lookup(const char*);
extern void _nrn_thread_reg(int, int, void(*)(Datum*));
void _nrn_thread_table_reg(int, nrn_thread_table_check_t);
extern void hoc_register_tolerance(int, HocStateTolerance*, Symbol***);
extern void _cvode_abstol( Symbol**, double*, int);

 extern "C" void _Gfluct_reg() {
	int _vectorized = 1;
  _initlists();
 	_pointtype = point_register_mech(_mechanism,
	 nrn_alloc,nrn_cur, nrn_jacob, nrn_state, nrn_init,
	 hoc_nrnpointerindex, 1,
	 _hoc_create_pnt, _hoc_destroy_pnt, _member_func);
 _mechtype = nrn_get_mechtype(_mechanism[1]);
 hoc_register_parm_default(_mechtype, &_parm_default);
     _nrn_setdata_reg(_mechtype, _setdata);
 #if NMODL_TEXT
  register_nmodl_text_and_filename(_mechtype);
#endif
   _nrn_mechanism_register_data_fields(_mechtype,
                                       _nrn_mechanism_field<double>{"E_e"} /* 0 */,
                                       _nrn_mechanism_field<double>{"E_i"} /* 1 */,
                                       _nrn_mechanism_field<double>{"g_e0"} /* 2 */,
                                       _nrn_mechanism_field<double>{"g_i0"} /* 3 */,
                                       _nrn_mechanism_field<double>{"std_e"} /* 4 */,
                                       _nrn_mechanism_field<double>{"std_i"} /* 5 */,
                                       _nrn_mechanism_field<double>{"tau_e"} /* 6 */,
                                       _nrn_mechanism_field<double>{"tau_i"} /* 7 */,
                                       _nrn_mechanism_field<double>{"i"} /* 8 */,
                                       _nrn_mechanism_field<double>{"g_e"} /* 9 */,
                                       _nrn_mechanism_field<double>{"g_i"} /* 10 */,
                                       _nrn_mechanism_field<double>{"g_e1"} /* 11 */,
                                       _nrn_mechanism_field<double>{"g_i1"} /* 12 */,
                                       _nrn_mechanism_field<double>{"D_e"} /* 13 */,
                                       _nrn_mechanism_field<double>{"D_i"} /* 14 */,
                                       _nrn_mechanism_field<double>{"exp_e"} /* 15 */,
                                       _nrn_mechanism_field<double>{"exp_i"} /* 16 */,
                                       _nrn_mechanism_field<double>{"amp_e"} /* 17 */,
                                       _nrn_mechanism_field<double>{"amp_i"} /* 18 */,
                                       _nrn_mechanism_field<double>{"v"} /* 19 */,
                                       _nrn_mechanism_field<double>{"_g"} /* 20 */,
                                       _nrn_mechanism_field<double*>{"_nd_area", "area"} /* 0 */,
                                       _nrn_mechanism_field<Point_process*>{"_pntproc", "pntproc"} /* 1 */,
                                       _nrn_mechanism_field<void*>{"rng", "random"} /* 2 */);
  hoc_register_prop_size(_mechtype, 21, 3);
  hoc_register_dparam_semantics(_mechtype, 0, "area");
  hoc_register_dparam_semantics(_mechtype, 1, "pntproc");
  hoc_register_dparam_semantics(_mechtype, 2, "random");
 	hoc_register_cvode(_mechtype, _ode_count, 0, 0, 0);
 
    hoc_register_var(hoc_scdoub, hoc_vdoub, hoc_intfunc);
 	ivoc_help("help ?1 Gfluct2 /home/diogo/Dropbox/Diogo/H01_Network_model/cod/GPU_test/Human_L23_NetPyNE-main/sim/mod/Gfluct.mod\n");
 hoc_register_limits(_mechtype, _hoc_parm_limits);
 hoc_register_units(_mechtype, _hoc_parm_units);
 }
static int _reset;
static const char *modelname = "Fluctuating conductances with a native Random123 stream";

static int error;
static int _ninits = 0;
static int _match_recurse=1;
static void _modl_cleanup(){ _match_recurse=1;}
static int oup(_internalthreadargsproto_);
 
static int  oup ( _internalthreadargsproto_ ) {
   if ( tau_e > 0.0 ) {
     g_e1 = exp_e * g_e1 + amp_e * nrnran123_normal ( rng ) ;
     }
   else {
     g_e1 = std_e * nrnran123_normal ( rng ) ;
     }
   if ( tau_i > 0.0 ) {
     g_i1 = exp_i * g_i1 + amp_i * nrnran123_normal ( rng ) ;
     }
   else {
     g_i1 = std_i * nrnran123_normal ( rng ) ;
     }
    return 0; }
 
static double _hoc_oup(void* _vptr) {
 double _r;
 Datum* _ppvar; Datum* _thread; NrnThread* _nt;
   auto* const _pnt = static_cast<Point_process*>(_vptr);
  auto* const _p = _pnt->_prop;
  if (!_p) {
    hoc_execerror("POINT_PROCESS data instance not valid", NULL);
  }
   _nrn_mechanism_cache_instance _ml_real{_p};
  auto* const _ml = &_ml_real;
  size_t const _iml{};
  _ppvar = _nrn_mechanism_access_dparam(_p);
  _thread = _extcall_thread.data();
  double* _globals = nullptr;
  if (gind != 0 && _thread != nullptr) { _globals = _thread[_gth].get<double*>(); }
  _nt = static_cast<NrnThread*>(_pnt->_vnt);
 _r = 1.;
 oup ( _threadargs_ );
 return(_r);
}
 
static int _ode_count(int _type){ hoc_execerror("Gfluct2", "cannot be used with CVODE"); return 0;}
 
static void _mech_inst_destruct(Prop* _prop) {
 Datum* _ppvar = _nrn_mechanism_access_dparam(_prop);
 nrnran123_deletestream(rng);
 }

static void initmodel(_internalthreadargsproto_) {
  int _i; double _save;{
 {
   nrnran123_setseq ( rng , 0.0 ) ;
   g_e1 = 0.0 ;
   g_i1 = 0.0 ;
   g_e = g_e0 ;
   g_i = g_i0 ;
   D_e = 0.0 ;
   D_i = 0.0 ;
   exp_e = 0.0 ;
   exp_i = 0.0 ;
   amp_e = 0.0 ;
   amp_i = 0.0 ;
   if ( tau_e > 0.0 ) {
     D_e = 2.0 * std_e * std_e / tau_e ;
     exp_e = exp ( - dt / tau_e ) ;
     amp_e = std_e * sqrt ( 1.0 - exp ( - 2.0 * dt / tau_e ) ) ;
     }
   if ( tau_i > 0.0 ) {
     D_i = 2.0 * std_i * std_i / tau_i ;
     exp_i = exp ( - dt / tau_i ) ;
     amp_i = std_i * sqrt ( 1.0 - exp ( - 2.0 * dt / tau_i ) ) ;
     }
   }
 
}
}

static void nrn_init(_nrn_model_sorted_token const& _sorted_token, NrnThread* _nt, Memb_list* _ml_arg, int _type){
_nrn_mechanism_cache_range _lmr{_sorted_token, *_nt, *_ml_arg, _type};
auto* const _vec_v = _nt->node_voltage_storage();
auto* const _ml = &_lmr;
Datum* _ppvar; Datum* _thread;
Node *_nd; double _v; int* _ni; int _iml, _cntml;
_ni = _ml_arg->_nodeindices;
_cntml = _ml_arg->_nodecount;
_thread = _ml_arg->_thread;
double* _globals = nullptr;
if (gind != 0 && _thread != nullptr) { _globals = _thread[_gth].get<double*>(); }
for (_iml = 0; _iml < _cntml; ++_iml) {
 _ppvar = _ml_arg->_pdata[_iml];
   _v = _vec_v[_ni[_iml]];
 v = _v;
 initmodel(_threadargs_);
}
}

static double _nrn_current(_internalthreadargsprotocomma_ double _v) {
double _current=0.; v=_v;
{ {
   g_e = g_e0 + g_e1 ;
   if ( g_e < 0.0 ) {
     g_e = 0.0 ;
     }
   g_i = g_i0 + g_i1 ;
   if ( g_i < 0.0 ) {
     g_i = 0.0 ;
     }
   i = g_e * ( v - E_e ) + g_i * ( v - E_i ) ;
   }
 _current += i;

} return _current;
}

static void nrn_cur(_nrn_model_sorted_token const& _sorted_token, NrnThread* _nt, Memb_list* _ml_arg, int _type) {
_nrn_mechanism_cache_range _lmr{_sorted_token, *_nt, *_ml_arg, _type};
auto const _vec_rhs = _nt->node_rhs_storage();
auto const _vec_sav_rhs = _nt->node_sav_rhs_storage();
auto const _vec_v = _nt->node_voltage_storage();
auto* const _ml = &_lmr;
Datum* _ppvar; Datum* _thread;
Node *_nd; int* _ni; double _rhs, _v; int _iml, _cntml;
_ni = _ml_arg->_nodeindices;
_cntml = _ml_arg->_nodecount;
_thread = _ml_arg->_thread;
double* _globals = nullptr;
if (gind != 0 && _thread != nullptr) { _globals = _thread[_gth].get<double*>(); }
for (_iml = 0; _iml < _cntml; ++_iml) {
 _ppvar = _ml_arg->_pdata[_iml];
   _v = _vec_v[_ni[_iml]];
 auto const _g_local = _nrn_current(_threadargscomma_ _v + .001);
 	{ _rhs = _nrn_current(_threadargscomma_ _v);
 	}
 _g = (_g_local - _rhs)/.001;
 _g *=  1.e2/(_nd_area);
 _rhs *= 1.e2/(_nd_area);
	 _vec_rhs[_ni[_iml]] -= _rhs;
 
}
 
}

static void nrn_jacob(_nrn_model_sorted_token const& _sorted_token, NrnThread* _nt, Memb_list* _ml_arg, int _type) {
_nrn_mechanism_cache_range _lmr{_sorted_token, *_nt, *_ml_arg, _type};
auto const _vec_d = _nt->node_d_storage();
auto const _vec_sav_d = _nt->node_sav_d_storage();
auto* const _ml = &_lmr;
Datum* _ppvar; Datum* _thread;
Node *_nd; int* _ni; int _iml, _cntml;
_ni = _ml_arg->_nodeindices;
_cntml = _ml_arg->_nodecount;
_thread = _ml_arg->_thread;
double* _globals = nullptr;
if (gind != 0 && _thread != nullptr) { _globals = _thread[_gth].get<double*>(); }
for (_iml = 0; _iml < _cntml; ++_iml) {
  _vec_d[_ni[_iml]] += _g;
 
}
 
}

static void nrn_state(_nrn_model_sorted_token const& _sorted_token, NrnThread* _nt, Memb_list* _ml_arg, int _type) {
_nrn_mechanism_cache_range _lmr{_sorted_token, *_nt, *_ml_arg, _type};
auto* const _vec_v = _nt->node_voltage_storage();
auto* const _ml = &_lmr;
Datum* _ppvar; Datum* _thread;
Node *_nd; double _v = 0.0; int* _ni;
_ni = _ml_arg->_nodeindices;
size_t _cntml = _ml_arg->_nodecount;
_thread = _ml_arg->_thread;
double* _globals = nullptr;
if (gind != 0 && _thread != nullptr) { _globals = _thread[_gth].get<double*>(); }
for (size_t _iml = 0; _iml < _cntml; ++_iml) {
 _ppvar = _ml_arg->_pdata[_iml];
 _nd = _ml_arg->_nodelist[_iml];
   _v = _vec_v[_ni[_iml]];
 v=_v;
{
 {  { oup(_threadargs_); }
  }}}

}

static void terminal(){}

static void _initlists(){
 int _i; static int _first = 1;
  if (!_first) return;
_first = 0;
}

#if NMODL_TEXT
static void register_nmodl_text_and_filename(int mech_type) {
    const char* nmodl_filename = "/home/diogo/Dropbox/Diogo/H01_Network_model/cod/GPU_test/Human_L23_NetPyNE-main/sim/mod/Gfluct.mod";
    const char* nmodl_file_text = 
  "TITLE Fluctuating conductances with a native Random123 stream\n"
  "\n"
  "COMMENT\n"
  "CoreNEURON 9.0.1-compatible Gfluct2 implementation.\n"
  "\n"
  "The Random123 stream is declared with RANDOM rng. Its identifiers are assigned\n"
  "from Python through the NMODLRandom wrapper:\n"
  "\n"
  "    ou.rng.set_ids(id1, id2, id3)\n"
  "    ou.rng.set_seq(0)\n"
  "\n"
  "Do not call random_setids() in this MOD file with CoreNEURON 9.0.1: that\n"
  "version's CoreNEURON translation can emit a call to an unavailable\n"
  "nrnran123_setids symbol. Also use random_normal(rng), without mean/std\n"
  "arguments, because the generated CoreNEURON helper accepts only the stream.\n"
  "ENDCOMMENT\n"
  "\n"
  "NEURON {\n"
  "    POINT_PROCESS Gfluct2\n"
  "    NONSPECIFIC_CURRENT i\n"
  "\n"
  "    RANGE E_e, E_i\n"
  "    RANGE g_e0, g_i0, std_e, std_i, tau_e, tau_i\n"
  "    RANGE g_e, g_i, g_e1, g_i1, D_e, D_i\n"
  "\n"
  "    RANDOM rng\n"
  "    THREADSAFE\n"
  "}\n"
  "\n"
  "UNITS {\n"
  "    (nA) = (nanoamp)\n"
  "    (mV) = (millivolt)\n"
  "    (umho) = (micromho)\n"
  "}\n"
  "\n"
  "PARAMETER {\n"
  "    dt              (ms)\n"
  "\n"
  "    E_e   = 0        (mV)\n"
  "    E_i   = -75      (mV)\n"
  "\n"
  "    g_e0  = 0.0121   (umho)\n"
  "    g_i0  = 0.0573   (umho)\n"
  "\n"
  "    std_e = 0.0030   (umho)\n"
  "    std_i = 0.0066   (umho)\n"
  "\n"
  "    tau_e = 2.728    (ms)\n"
  "    tau_i = 10.49    (ms)\n"
  "}\n"
  "\n"
  "ASSIGNED {\n"
  "    v       (mV)\n"
  "    i       (nA)\n"
  "\n"
  "    g_e     (umho)\n"
  "    g_i     (umho)\n"
  "    g_e1    (umho)\n"
  "    g_i1    (umho)\n"
  "\n"
  "    D_e     (umho umho/ms)\n"
  "    D_i     (umho umho/ms)\n"
  "\n"
  "    exp_e\n"
  "    exp_i\n"
  "    amp_e   (umho)\n"
  "    amp_i   (umho)\n"
  "}\n"
  "\n"
  "INITIAL {\n"
  "    : IDs are set from Python before finitialize/CoreNEURON transfer.\n"
  "    : Reset the counter on every initialization.\n"
  "    random_setseq(rng, 0)\n"
  "\n"
  "    g_e1 = 0\n"
  "    g_i1 = 0\n"
  "    g_e  = g_e0\n"
  "    g_i  = g_i0\n"
  "\n"
  "    D_e   = 0\n"
  "    D_i   = 0\n"
  "    exp_e = 0\n"
  "    exp_i = 0\n"
  "    amp_e = 0\n"
  "    amp_i = 0\n"
  "\n"
  "    if (tau_e > 0) {\n"
  "        D_e   = 2 * std_e * std_e / tau_e\n"
  "        exp_e = exp(-dt/tau_e)\n"
  "        amp_e = std_e * sqrt(1 - exp(-2*dt/tau_e))\n"
  "    }\n"
  "\n"
  "    if (tau_i > 0) {\n"
  "        D_i   = 2 * std_i * std_i / tau_i\n"
  "        exp_i = exp(-dt/tau_i)\n"
  "        amp_i = std_i * sqrt(1 - exp(-2*dt/tau_i))\n"
  "    }\n"
  "}\n"
  "\n"
  "BREAKPOINT {\n"
  "    SOLVE oup\n"
  "\n"
  "    g_e = g_e0 + g_e1\n"
  "    if (g_e < 0) {\n"
  "        g_e = 0\n"
  "    }\n"
  "\n"
  "    g_i = g_i0 + g_i1\n"
  "    if (g_i < 0) {\n"
  "        g_i = 0\n"
  "    }\n"
  "\n"
  "    i = g_e * (v - E_e) + g_i * (v - E_i)\n"
  "}\n"
  "\n"
  "PROCEDURE oup() {\n"
  "    if (tau_e > 0) {\n"
  "        g_e1 = exp_e * g_e1 + amp_e * random_normal(rng)\n"
  "    } else {\n"
  "        g_e1 = std_e * random_normal(rng)\n"
  "    }\n"
  "\n"
  "    if (tau_i > 0) {\n"
  "        g_i1 = exp_i * g_i1 + amp_i * random_normal(rng)\n"
  "    } else {\n"
  "        g_i1 = std_i * random_normal(rng)\n"
  "    }\n"
  "}\n"
  ;
    hoc_reg_nmodl_filename(mech_type, nmodl_filename);
    hoc_reg_nmodl_text(mech_type, nmodl_file_text);
}
#endif
