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
static constexpr auto number_of_floating_point_variables = 18;
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
 
#define nrn_init _nrn_init__ProbUDFsyn
#define _nrn_initial _nrn_initial__ProbUDFsyn
#define nrn_cur _nrn_cur__ProbUDFsyn
#define _nrn_current _nrn_current__ProbUDFsyn
#define nrn_jacob _nrn_jacob__ProbUDFsyn
#define nrn_state _nrn_state__ProbUDFsyn
#define _net_receive _net_receive__ProbUDFsyn 
#define state state__ProbUDFsyn 
 
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
#define tau_r _ml->template fpfield<0>(_iml)
#define tau_r_columnindex 0
#define tau_d _ml->template fpfield<1>(_iml)
#define tau_d_columnindex 1
#define Use _ml->template fpfield<2>(_iml)
#define Use_columnindex 2
#define Dep _ml->template fpfield<3>(_iml)
#define Dep_columnindex 3
#define Fac _ml->template fpfield<4>(_iml)
#define Fac_columnindex 4
#define u0 _ml->template fpfield<5>(_iml)
#define u0_columnindex 5
#define e _ml->template fpfield<6>(_iml)
#define e_columnindex 6
#define gmax _ml->template fpfield<7>(_iml)
#define gmax_columnindex 7
#define i _ml->template fpfield<8>(_iml)
#define i_columnindex 8
#define g _ml->template fpfield<9>(_iml)
#define g_columnindex 9
#define A _ml->template fpfield<10>(_iml)
#define A_columnindex 10
#define B _ml->template fpfield<11>(_iml)
#define B_columnindex 11
#define factor _ml->template fpfield<12>(_iml)
#define factor_columnindex 12
#define DA _ml->template fpfield<13>(_iml)
#define DA_columnindex 13
#define DB _ml->template fpfield<14>(_iml)
#define DB_columnindex 14
#define v _ml->template fpfield<15>(_iml)
#define v_columnindex 15
#define _g _ml->template fpfield<16>(_iml)
#define _g_columnindex 16
#define _tsav _ml->template fpfield<17>(_iml)
#define _tsav_columnindex 17
#define _nd_area *_ml->dptr_field<0>(_iml)
 
 //RANDOM variables 
 #define rng	(nrnran123_State*)_ppvar[2].get<void*>()
 #define _p_rng _ppvar[2].literal_value<void*>()
 
 /* Thread safe. No static _ml, _iml or _ppvar. */
 static int hoc_nrnpointerindex =  -1;
 static _nrn_mechanism_std_vector<Datum> _extcall_thread;
 /* external NEURON variables */
 /* declaration of user functions */
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
 {"tau_r", "ms"},
 {"tau_d", "ms"},
 {"Use", "1"},
 {"Dep", "ms"},
 {"Fac", "ms"},
 {"u0", "1"},
 {"e", "mV"},
 {"gmax", "uS"},
 {"i", "nA"},
 {"g", "uS"},
 {0, 0}
};
 static double A0 = 0;
 static double B0 = 0;
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
static void _ode_map(Prop*, int, neuron::container::data_handle<double>*, neuron::container::data_handle<double>*, double*, int);
static void _ode_spec(_nrn_model_sorted_token const&, NrnThread*, Memb_list*, int);
static void _ode_matsol(_nrn_model_sorted_token const&, NrnThread*, Memb_list*, int);
 
#define _cvode_ieq _ppvar[3].literal_value<int>()
 static void _ode_matsol_instance1(_internalthreadargsproto_);
 /* connect range variables in _p that hoc is supposed to know about */
 static const char *_mechanism[] = {
 "7.7.0",
"ProbUDFsyn",
 "tau_r",
 "tau_d",
 "Use",
 "Dep",
 "Fac",
 "u0",
 "e",
 "gmax",
 0,
 "i",
 "g",
 0,
 "A",
 "B",
 0,
 0};
 
 /* Used by NrnProperty */
 static _nrn_mechanism_std_vector<double> _parm_default{
     0.2, /* tau_r */
     1.7, /* tau_d */
     1, /* Use */
     100, /* Dep */
     10, /* Fac */
     0, /* u0 */
     0, /* e */
     0.001, /* gmax */
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
   _ppvar = nrn_prop_datum_alloc(_mechtype, 4, _prop);
    _nrn_mechanism_access_dparam(_prop) = _ppvar;
     _nrn_mechanism_cache_instance _ml_real{_prop};
    auto* const _ml = &_ml_real;
    size_t const _iml{};
    assert(_nrn_mechanism_get_num_vars(_prop) == 18);
 	/*initialize range parameters*/
 	tau_r = _parm_default[0]; /* 0.2 */
 	tau_d = _parm_default[1]; /* 1.7 */
 	Use = _parm_default[2]; /* 1 */
 	Dep = _parm_default[3]; /* 100 */
 	Fac = _parm_default[4]; /* 10 */
 	u0 = _parm_default[5]; /* 0 */
 	e = _parm_default[6]; /* 0 */
 	gmax = _parm_default[7]; /* 0.001 */
  }
 	 assert(_nrn_mechanism_get_num_vars(_prop) == 18);
 	_nrn_mechanism_access_dparam(_prop) = _ppvar;
 	/*connect ionic variables to this model*/
 _p_rng = (void*)nrnran123_newstream();
 nrn_mech_inst_destruct[_mechtype] = _mech_inst_destruct;
 
}
 static void _initlists();
  /* some states have an absolute tolerance */
 static Symbol** _atollist;
 static HocStateTolerance _hoc_state_tol[] = {
 {0, 0}
};
 static void _net_receive(Point_process*, double*, double);
 extern Symbol* hoc_lookup(const char*);
extern void _nrn_thread_reg(int, int, void(*)(Datum*));
void _nrn_thread_table_reg(int, nrn_thread_table_check_t);
extern void hoc_register_tolerance(int, HocStateTolerance*, Symbol***);
extern void _cvode_abstol( Symbol**, double*, int);

 extern "C" void _ProbUDFsyn_reg() {
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
                                       _nrn_mechanism_field<double>{"tau_r"} /* 0 */,
                                       _nrn_mechanism_field<double>{"tau_d"} /* 1 */,
                                       _nrn_mechanism_field<double>{"Use"} /* 2 */,
                                       _nrn_mechanism_field<double>{"Dep"} /* 3 */,
                                       _nrn_mechanism_field<double>{"Fac"} /* 4 */,
                                       _nrn_mechanism_field<double>{"u0"} /* 5 */,
                                       _nrn_mechanism_field<double>{"e"} /* 6 */,
                                       _nrn_mechanism_field<double>{"gmax"} /* 7 */,
                                       _nrn_mechanism_field<double>{"i"} /* 8 */,
                                       _nrn_mechanism_field<double>{"g"} /* 9 */,
                                       _nrn_mechanism_field<double>{"A"} /* 10 */,
                                       _nrn_mechanism_field<double>{"B"} /* 11 */,
                                       _nrn_mechanism_field<double>{"factor"} /* 12 */,
                                       _nrn_mechanism_field<double>{"DA"} /* 13 */,
                                       _nrn_mechanism_field<double>{"DB"} /* 14 */,
                                       _nrn_mechanism_field<double>{"v"} /* 15 */,
                                       _nrn_mechanism_field<double>{"_g"} /* 16 */,
                                       _nrn_mechanism_field<double>{"_tsav"} /* 17 */,
                                       _nrn_mechanism_field<double*>{"_nd_area", "area"} /* 0 */,
                                       _nrn_mechanism_field<Point_process*>{"_pntproc", "pntproc"} /* 1 */,
                                       _nrn_mechanism_field<void*>{"rng", "random"} /* 2 */,
                                       _nrn_mechanism_field<int>{"_cvode_ieq", "cvodeieq"} /* 3 */);
  hoc_register_prop_size(_mechtype, 18, 4);
  hoc_register_dparam_semantics(_mechtype, 0, "area");
  hoc_register_dparam_semantics(_mechtype, 1, "pntproc");
  hoc_register_dparam_semantics(_mechtype, 2, "random");
  hoc_register_dparam_semantics(_mechtype, 3, "cvodeieq");
 	hoc_register_cvode(_mechtype, _ode_count, _ode_map, _ode_spec, _ode_matsol);
 	hoc_register_tolerance(_mechtype, _hoc_state_tol, &_atollist);
 pnt_receive[_mechtype] = _net_receive;
 pnt_receive_size[_mechtype] = 5;
 
    hoc_register_var(hoc_scdoub, hoc_vdoub, hoc_intfunc);
 	ivoc_help("help ?1 ProbUDFsyn /home/diogo/Dropbox/Diogo/H01_Network_model/cod/GPU_test/Human_L23_NetPyNE-main/sim/mod/ProbUDFsyn.mod\n");
 hoc_register_limits(_mechtype, _hoc_parm_limits);
 hoc_register_units(_mechtype, _hoc_parm_units);
 }
static int _reset;
static const char *modelname = "Dual-exponential inhibitory synapse with probabilistic short-term plasticity";

static int error;
static int _ninits = 0;
static int _match_recurse=1;
static void _modl_cleanup(){ _match_recurse=1;}
 
static int _ode_spec1(_internalthreadargsproto_);
/*static int _ode_matsol1(_internalthreadargsproto_);*/
 static neuron::container::field_index _slist1[2], _dlist1[2];
 static int state(_internalthreadargsproto_);
 
/*CVODE*/
 static int _ode_spec1 (_internalthreadargsproto_) {int _reset = 0; {
   DA = - A / tau_r ;
   DB = - B / tau_d ;
   }
 return _reset;
}
 static int _ode_matsol1 (_internalthreadargsproto_) {
 DA = DA  / (1. - dt*( ( - 1.0 ) / tau_r )) ;
 DB = DB  / (1. - dt*( ( - 1.0 ) / tau_d )) ;
  return 0;
}
 /*END CVODE*/
 static int state (_internalthreadargsproto_) { {
    A = A + (1. - exp(dt*(( - 1.0 ) / tau_r)))*(- ( 0.0 ) / ( ( - 1.0 ) / tau_r ) - A) ;
    B = B + (1. - exp(dt*(( - 1.0 ) / tau_d)))*(- ( 0.0 ) / ( ( - 1.0 ) / tau_d ) - B) ;
   }
  return 0;
}
 
static void _net_receive (Point_process* _pnt, double* _args, double _lflag) 
{  Prop* _p; Datum* _ppvar; Datum* _thread; NrnThread* _nt;
   _nrn_mechanism_cache_instance _ml_real{_pnt->_prop};
  auto* const _ml = &_ml_real;
  size_t const _iml{};
   _thread = nullptr; double* _globals = nullptr; _nt = (NrnThread*)_pnt->_vnt;   _ppvar = _nrn_mechanism_access_dparam(_pnt->_prop);
  if (_tsav > t){ hoc_execerror(hoc_object_name(_pnt->ob), ":Event arrived out of order. Must call ParallelContext.set_maxstep AFTER assigning minimum NetCon.delay");}
 _tsav = t; {
   double _lPv_tmp , _lPr ;
 if ( _args[4]  == 0.0 ) {
     _args[1] = 1.0 ;
     _args[2] = u0 ;
     _args[3] = 0.0 ;
     _args[4] = 1.0 ;
     }
   if ( Fac > 0.0 ) {
     _args[2] = _args[2] * exp ( - ( t - _args[3] ) / Fac ) ;
     _args[2] = _args[2] + Use * ( 1.0 - _args[2] ) ;
     }
   else {
     _args[2] = Use ;
     }
   _lPv_tmp = 1.0 - ( 1.0 - _args[1] ) * exp ( - ( t - _args[3] ) / Dep ) ;
   _lPr = _args[2] * _lPv_tmp ;
   _lPv_tmp = _lPv_tmp - _args[2] * _lPv_tmp ;
   if ( nrnran123_negexp ( rng ) < _lPr ) {
     _args[3] = t ;
     _args[1] = _lPv_tmp ;
       if (nrn_netrec_state_adjust && !cvode_active_){
    /* discon state adjustment for cnexp case (rate uses no local variable) */
    double __state = A;
    double __primary = (A + _args[0] * factor) - __state;
     __primary += ( 1. - exp( 0.5*dt*( ( - 1.0 ) / tau_r ) ) )*( - ( 0.0 ) / ( ( - 1.0 ) / tau_r ) - __primary );
    A += __primary;
  } else {
 A = A + _args[0] * factor ;
       }
   if (nrn_netrec_state_adjust && !cvode_active_){
    /* discon state adjustment for cnexp case (rate uses no local variable) */
    double __state = B;
    double __primary = (B + _args[0] * factor) - __state;
     __primary += ( 1. - exp( 0.5*dt*( ( - 1.0 ) / tau_d ) ) )*( - ( 0.0 ) / ( ( - 1.0 ) / tau_d ) - __primary );
    B += __primary;
  } else {
 B = B + _args[0] * factor ;
       }
 }
   } }
 
static int _ode_count(int _type){ return 2;}
 
static void _ode_spec(_nrn_model_sorted_token const& _sorted_token, NrnThread* _nt, Memb_list* _ml_arg, int _type) {
   Datum* _ppvar;
   size_t _iml;   _nrn_mechanism_cache_range* _ml;   Node* _nd{};
  double _v{};
  int _cntml;
  _nrn_mechanism_cache_range _lmr{_sorted_token, *_nt, *_ml_arg, _type};
  _ml = &_lmr;
  _cntml = _ml_arg->_nodecount;
  Datum *_thread{_ml_arg->_thread};
  double* _globals = nullptr;
  if (gind != 0 && _thread != nullptr) { _globals = _thread[_gth].get<double*>(); }
  for (_iml = 0; _iml < _cntml; ++_iml) {
    _ppvar = _ml_arg->_pdata[_iml];
    _nd = _ml_arg->_nodelist[_iml];
    v = NODEV(_nd);
     _ode_spec1 (_threadargs_);
 }}
 
static void _ode_map(Prop* _prop, int _ieq, neuron::container::data_handle<double>* _pv, neuron::container::data_handle<double>* _pvdot, double* _atol, int _type) { 
  Datum* _ppvar;
  _ppvar = _nrn_mechanism_access_dparam(_prop);
  _cvode_ieq = _ieq;
  for (int _i=0; _i < 2; ++_i) {
    _pv[_i] = _nrn_mechanism_get_param_handle(_prop, _slist1[_i]);
    _pvdot[_i] = _nrn_mechanism_get_param_handle(_prop, _dlist1[_i]);
    _cvode_abstol(_atollist, _atol, _i);
  }
 }
 
static void _ode_matsol_instance1(_internalthreadargsproto_) {
 _ode_matsol1 (_threadargs_);
 }
 
static void _ode_matsol(_nrn_model_sorted_token const& _sorted_token, NrnThread* _nt, Memb_list* _ml_arg, int _type) {
   Datum* _ppvar;
   size_t _iml;   _nrn_mechanism_cache_range* _ml;   Node* _nd{};
  double _v{};
  int _cntml;
  _nrn_mechanism_cache_range _lmr{_sorted_token, *_nt, *_ml_arg, _type};
  _ml = &_lmr;
  _cntml = _ml_arg->_nodecount;
  Datum *_thread{_ml_arg->_thread};
  double* _globals = nullptr;
  if (gind != 0 && _thread != nullptr) { _globals = _thread[_gth].get<double*>(); }
  for (_iml = 0; _iml < _cntml; ++_iml) {
    _ppvar = _ml_arg->_pdata[_iml];
    _nd = _ml_arg->_nodelist[_iml];
    v = NODEV(_nd);
 _ode_matsol_instance1(_threadargs_);
 }}
 
static void _mech_inst_destruct(Prop* _prop) {
 Datum* _ppvar = _nrn_mechanism_access_dparam(_prop);
 nrnran123_deletestream(rng);
 }

static void initmodel(_internalthreadargsproto_) {
  int _i; double _save;{
  A = A0;
  B = B0;
 {
   double _ltp ;
 A = 0.0 ;
   B = 0.0 ;
   _ltp = ( tau_r * tau_d ) / ( tau_d - tau_r ) * log ( tau_d / tau_r ) ;
   factor = 1.0 / ( - exp ( - _ltp / tau_r ) + exp ( - _ltp / tau_d ) ) ;
   nrnran123_setseq ( rng , 0.0 ) ;
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
 _tsav = -1e20;
   _v = _vec_v[_ni[_iml]];
 v = _v;
 initmodel(_threadargs_);
}
}

static double _nrn_current(_internalthreadargsprotocomma_ double _v) {
double _current=0.; v=_v;
{ {
   g = gmax * ( B - A ) ;
   i = g * ( v - e ) ;
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
 {   state(_threadargs_);
  }}}

}

static void terminal(){}

static void _initlists(){
 int _i; static int _first = 1;
  if (!_first) return;
 _slist1[0] = {A_columnindex, 0};  _dlist1[0] = {DA_columnindex, 0};
 _slist1[1] = {B_columnindex, 0};  _dlist1[1] = {DB_columnindex, 0};
_first = 0;
}

#if NMODL_TEXT
static void register_nmodl_text_and_filename(int mech_type) {
    const char* nmodl_filename = "/home/diogo/Dropbox/Diogo/H01_Network_model/cod/GPU_test/Human_L23_NetPyNE-main/sim/mod/ProbUDFsyn.mod";
    const char* nmodl_file_text = 
  "TITLE Dual-exponential inhibitory synapse with probabilistic short-term plasticity\n"
  "\n"
  "COMMENT\n"
  "CoreNEURON 9.0.1 GPU-compatible port of ProbUDFsyn.\n"
  "\n"
  "Important implementation details:\n"
  "  * native NMODL RANDOM stream; no POINTER or VERBATIM RNG plumbing\n"
  "  * no nested INITIAL block inside NET_RECEIVE\n"
  "  * per-NetCon plasticity state is initialized lazily on the first event\n"
  "  * Pv_tmp and Pr are event-local temporaries, not persistent NetCon weights\n"
  "  * random_negexp(rng) preserves the effective distribution used by the\n"
  "    original mechanism (Random.negexp(1) / exprand(1))\n"
  "ENDCOMMENT\n"
  "\n"
  "NEURON {\n"
  "    POINT_PROCESS ProbUDFsyn\n"
  "\n"
  "    RANGE tau_r, tau_d\n"
  "    RANGE Use, Dep, Fac, u0\n"
  "    RANGE i, g, e, gmax\n"
  "\n"
  "    NONSPECIFIC_CURRENT i\n"
  "\n"
  "    RANDOM rng\n"
  "    THREADSAFE\n"
  "}\n"
  "\n"
  "PARAMETER {\n"
  "    tau_r = 0.2 (ms)\n"
  "    tau_d = 1.7 (ms)\n"
  "\n"
  "    Use = 1.0 (1)\n"
  "    Dep = 100 (ms)\n"
  "    Fac = 10  (ms)\n"
  "    u0  = 0   (1)\n"
  "\n"
  "    e = 0 (mV)\n"
  "    gmax = 0.001 (uS)\n"
  "}\n"
  "\n"
  "ASSIGNED {\n"
  "    v (mV)\n"
  "    i (nA)\n"
  "    g (uS)\n"
  "    factor (1)\n"
  "}\n"
  "\n"
  "STATE {\n"
  "    A\n"
  "    B\n"
  "}\n"
  "\n"
  "INITIAL {\n"
  "    LOCAL tp\n"
  "\n"
  "    A = 0\n"
  "    B = 0\n"
  "\n"
  "    tp = (tau_r * tau_d) / (tau_d - tau_r) * log(tau_d / tau_r)\n"
  "    factor = 1 / (-exp(-tp / tau_r) + exp(-tp / tau_d))\n"
  "\n"
  "    : Keep configured Random123 IDs, but restart the sequence at finitialize().\n"
  "    random_setseq(rng, 0)\n"
  "}\n"
  "\n"
  "BREAKPOINT {\n"
  "    SOLVE state METHOD cnexp\n"
  "\n"
  "    g = gmax * (B - A)\n"
  "    i = g * (v - e)\n"
  "}\n"
  "\n"
  "DERIVATIVE state {\n"
  "    A' = -A / tau_r\n"
  "    B' = -B / tau_d\n"
  "}\n"
  "\n"
  "NET_RECEIVE (weight, Pv, u, tsyn (ms), initialized) {\n"
  "    LOCAL Pv_tmp, Pr\n"
  "\n"
  "    : With no NET_RECEIVE INITIAL block, NEURON resets all weight slots after\n"
  "    : weight[0] to zero at finitialize().  Initialize this NetCon's STP state\n"
  "    : when its first event is delivered.  The original nested INITIAL block\n"
  "    : ran at t=0, hence tsyn starts at 0 here as well.\n"
  "    if (initialized == 0) {\n"
  "        Pv = 1\n"
  "        u = u0\n"
  "        tsyn = 0\n"
  "        initialized = 1\n"
  "    }\n"
  "\n"
  "    : Facilitation (Fuhrmann et al., Eq. 2).\n"
  "    if (Fac > 0) {\n"
  "        u = u * exp(-(t - tsyn) / Fac)\n"
  "        u = u + Use * (1 - u)\n"
  "    } else {\n"
  "        u = Use\n"
  "    }\n"
  "\n"
  "    : Vesicle availability and release probability (Eq. 3).\n"
  "    Pv_tmp = 1 - (1 - Pv) * exp(-(t - tsyn) / Dep)\n"
  "    Pr = u * Pv_tmp\n"
  "    Pv_tmp = Pv_tmp - u * Pv_tmp\n"
  "\n"
  "    : Preserve the original mechanism's negative-exponential random draw.\n"
  "    if (random_negexp(rng) < Pr) {\n"
  "        tsyn = t\n"
  "        Pv = Pv_tmp\n"
  "\n"
  "        A = A + weight * factor\n"
  "        B = B + weight * factor\n"
  "    }\n"
  "}\n"
  ;
    hoc_reg_nmodl_filename(mech_type, nmodl_filename);
    hoc_reg_nmodl_text(mech_type, nmodl_file_text);
}
#endif
