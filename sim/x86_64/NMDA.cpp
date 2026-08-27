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
static constexpr auto number_of_datum_variables = 2;
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
 
#define nrn_init _nrn_init__NMDA
#define _nrn_initial _nrn_initial__NMDA
#define nrn_cur _nrn_cur__NMDA
#define _nrn_current _nrn_current__NMDA
#define nrn_jacob _nrn_jacob__NMDA
#define nrn_state _nrn_state__NMDA
#define _net_receive _net_receive__NMDA 
#define state state__NMDA 
 
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
#define tau_r_NMDA _ml->template fpfield<0>(_iml)
#define tau_r_NMDA_columnindex 0
#define tau_d_NMDA _ml->template fpfield<1>(_iml)
#define tau_d_NMDA_columnindex 1
#define n_NMDA _ml->template fpfield<2>(_iml)
#define n_NMDA_columnindex 2
#define gama_NMDA _ml->template fpfield<3>(_iml)
#define gama_NMDA_columnindex 3
#define e _ml->template fpfield<4>(_iml)
#define e_columnindex 4
#define i _ml->template fpfield<5>(_iml)
#define i_columnindex 5
#define i_NMDA _ml->template fpfield<6>(_iml)
#define i_NMDA_columnindex 6
#define g_NMDA _ml->template fpfield<7>(_iml)
#define g_NMDA_columnindex 7
#define A_NMDA _ml->template fpfield<8>(_iml)
#define A_NMDA_columnindex 8
#define B_NMDA _ml->template fpfield<9>(_iml)
#define B_NMDA_columnindex 9
#define factor_NMDA _ml->template fpfield<10>(_iml)
#define factor_NMDA_columnindex 10
#define mggate _ml->template fpfield<11>(_iml)
#define mggate_columnindex 11
#define tp_NMDA _ml->template fpfield<12>(_iml)
#define tp_NMDA_columnindex 12
#define DA_NMDA _ml->template fpfield<13>(_iml)
#define DA_NMDA_columnindex 13
#define DB_NMDA _ml->template fpfield<14>(_iml)
#define DB_NMDA_columnindex 14
#define v _ml->template fpfield<15>(_iml)
#define v_columnindex 15
#define _g _ml->template fpfield<16>(_iml)
#define _g_columnindex 16
#define _tsav _ml->template fpfield<17>(_iml)
#define _tsav_columnindex 17
#define _nd_area *_ml->dptr_field<0>(_iml)
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
#define mg mg_NMDA
 double mg = 1;
 /* some parameters have upper and lower limits */
 static HocParmLimits _hoc_parm_limits[] = {
 {0, 0, 0}
};
 static HocParmUnits _hoc_parm_units[] = {
 {"mg_NMDA", "mM"},
 {"tau_r_NMDA", "ms"},
 {"tau_d_NMDA", "ms"},
 {"n_NMDA", "/mM"},
 {"gama_NMDA", "/mV"},
 {"e", "mV"},
 {"A_NMDA", "uS"},
 {"B_NMDA", "uS"},
 {"i", "nA"},
 {"i_NMDA", "nA"},
 {"g_NMDA", "uS"},
 {0, 0}
};
 static double A_NMDA0 = 0;
 static double B_NMDA0 = 0;
 static double delta_t = 0.01;
 /* connect global user variables to hoc */
 static DoubScal hoc_scdoub[] = {
 {"mg_NMDA", &mg_NMDA},
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
 
#define _cvode_ieq _ppvar[2].literal_value<int>()
 static void _ode_matsol_instance1(_internalthreadargsproto_);
 /* connect range variables in _p that hoc is supposed to know about */
 static const char *_mechanism[] = {
 "7.7.0",
"NMDA",
 "tau_r_NMDA",
 "tau_d_NMDA",
 "n_NMDA",
 "gama_NMDA",
 "e",
 0,
 "i",
 "i_NMDA",
 "g_NMDA",
 0,
 "A_NMDA",
 "B_NMDA",
 0,
 0};
 
 /* Used by NrnProperty */
 static _nrn_mechanism_std_vector<double> _parm_default{
     0.3, /* tau_r_NMDA */
     43, /* tau_d_NMDA */
     0.28011, /* n_NMDA */
     0.062, /* gama_NMDA */
     0, /* e */
 }; 
 
 
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
    assert(_nrn_mechanism_get_num_vars(_prop) == 18);
 	/*initialize range parameters*/
 	tau_r_NMDA = _parm_default[0]; /* 0.3 */
 	tau_d_NMDA = _parm_default[1]; /* 43 */
 	n_NMDA = _parm_default[2]; /* 0.28011 */
 	gama_NMDA = _parm_default[3]; /* 0.062 */
 	e = _parm_default[4]; /* 0 */
  }
 	 assert(_nrn_mechanism_get_num_vars(_prop) == 18);
 	_nrn_mechanism_access_dparam(_prop) = _ppvar;
 	/*connect ionic variables to this model*/
 
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

 extern "C" void _NMDA_reg() {
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
                                       _nrn_mechanism_field<double>{"tau_r_NMDA"} /* 0 */,
                                       _nrn_mechanism_field<double>{"tau_d_NMDA"} /* 1 */,
                                       _nrn_mechanism_field<double>{"n_NMDA"} /* 2 */,
                                       _nrn_mechanism_field<double>{"gama_NMDA"} /* 3 */,
                                       _nrn_mechanism_field<double>{"e"} /* 4 */,
                                       _nrn_mechanism_field<double>{"i"} /* 5 */,
                                       _nrn_mechanism_field<double>{"i_NMDA"} /* 6 */,
                                       _nrn_mechanism_field<double>{"g_NMDA"} /* 7 */,
                                       _nrn_mechanism_field<double>{"A_NMDA"} /* 8 */,
                                       _nrn_mechanism_field<double>{"B_NMDA"} /* 9 */,
                                       _nrn_mechanism_field<double>{"factor_NMDA"} /* 10 */,
                                       _nrn_mechanism_field<double>{"mggate"} /* 11 */,
                                       _nrn_mechanism_field<double>{"tp_NMDA"} /* 12 */,
                                       _nrn_mechanism_field<double>{"DA_NMDA"} /* 13 */,
                                       _nrn_mechanism_field<double>{"DB_NMDA"} /* 14 */,
                                       _nrn_mechanism_field<double>{"v"} /* 15 */,
                                       _nrn_mechanism_field<double>{"_g"} /* 16 */,
                                       _nrn_mechanism_field<double>{"_tsav"} /* 17 */,
                                       _nrn_mechanism_field<double*>{"_nd_area", "area"} /* 0 */,
                                       _nrn_mechanism_field<Point_process*>{"_pntproc", "pntproc"} /* 1 */,
                                       _nrn_mechanism_field<int>{"_cvode_ieq", "cvodeieq"} /* 2 */);
  hoc_register_prop_size(_mechtype, 18, 3);
  hoc_register_dparam_semantics(_mechtype, 0, "area");
  hoc_register_dparam_semantics(_mechtype, 1, "pntproc");
  hoc_register_dparam_semantics(_mechtype, 2, "cvodeieq");
 	hoc_register_cvode(_mechtype, _ode_count, _ode_map, _ode_spec, _ode_matsol);
 	hoc_register_tolerance(_mechtype, _hoc_state_tol, &_atollist);
 pnt_receive[_mechtype] = _net_receive;
 pnt_receive_size[_mechtype] = 1;
 
    hoc_register_var(hoc_scdoub, hoc_vdoub, hoc_intfunc);
 	ivoc_help("help ?1 NMDA /home/diogo/Dropbox/Diogo/H01_Network_model/cod/GPU_test/Human_L23_NetPyNE-main/sim/mod/NMDA.mod\n");
 hoc_register_limits(_mechtype, _hoc_parm_limits);
 hoc_register_units(_mechtype, _hoc_parm_units);
 }
static int _reset;
static const char *modelname = "NMDA receptor with voltage-dependent Mg2+ block";

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
   DA_NMDA = - A_NMDA / tau_r_NMDA ;
   DB_NMDA = - B_NMDA / tau_d_NMDA ;
   }
 return _reset;
}
 static int _ode_matsol1 (_internalthreadargsproto_) {
 DA_NMDA = DA_NMDA  / (1. - dt*( ( - 1.0 ) / tau_r_NMDA )) ;
 DB_NMDA = DB_NMDA  / (1. - dt*( ( - 1.0 ) / tau_d_NMDA )) ;
  return 0;
}
 /*END CVODE*/
 static int state (_internalthreadargsproto_) { {
    A_NMDA = A_NMDA + (1. - exp(dt*(( - 1.0 ) / tau_r_NMDA)))*(- ( 0.0 ) / ( ( - 1.0 ) / tau_r_NMDA ) - A_NMDA) ;
    B_NMDA = B_NMDA + (1. - exp(dt*(( - 1.0 ) / tau_d_NMDA)))*(- ( 0.0 ) / ( ( - 1.0 ) / tau_d_NMDA ) - B_NMDA) ;
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
     if (nrn_netrec_state_adjust && !cvode_active_){
    /* discon state adjustment for cnexp case (rate uses no local variable) */
    double __state = A_NMDA;
    double __primary = (A_NMDA + _args[0] * factor_NMDA) - __state;
     __primary += ( 1. - exp( 0.5*dt*( ( - 1.0 ) / tau_r_NMDA ) ) )*( - ( 0.0 ) / ( ( - 1.0 ) / tau_r_NMDA ) - __primary );
    A_NMDA += __primary;
  } else {
 A_NMDA = A_NMDA + _args[0] * factor_NMDA ;
     }
   if (nrn_netrec_state_adjust && !cvode_active_){
    /* discon state adjustment for cnexp case (rate uses no local variable) */
    double __state = B_NMDA;
    double __primary = (B_NMDA + _args[0] * factor_NMDA) - __state;
     __primary += ( 1. - exp( 0.5*dt*( ( - 1.0 ) / tau_d_NMDA ) ) )*( - ( 0.0 ) / ( ( - 1.0 ) / tau_d_NMDA ) - __primary );
    B_NMDA += __primary;
  } else {
 B_NMDA = B_NMDA + _args[0] * factor_NMDA ;
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

static void initmodel(_internalthreadargsproto_) {
  int _i; double _save;{
  A_NMDA = A_NMDA0;
  B_NMDA = B_NMDA0;
 {
   A_NMDA = 0.0 ;
   B_NMDA = 0.0 ;
   tp_NMDA = ( tau_r_NMDA * tau_d_NMDA ) / ( tau_d_NMDA - tau_r_NMDA ) * log ( tau_d_NMDA / tau_r_NMDA ) ;
   factor_NMDA = 1.0 / ( - exp ( - tp_NMDA / tau_r_NMDA ) + exp ( - tp_NMDA / tau_d_NMDA ) ) ;
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
   mggate = 1.0 / ( 1.0 + exp ( gama_NMDA * ( - v ) ) * ( mg / 3.57 ) ) ;
   g_NMDA = ( B_NMDA - A_NMDA ) * mggate ;
   i_NMDA = g_NMDA * ( v - e ) ;
   i = i_NMDA ;
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
 _slist1[0] = {A_NMDA_columnindex, 0};  _dlist1[0] = {DA_NMDA_columnindex, 0};
 _slist1[1] = {B_NMDA_columnindex, 0};  _dlist1[1] = {DB_NMDA_columnindex, 0};
_first = 0;
}

#if NMODL_TEXT
static void register_nmodl_text_and_filename(int mech_type) {
    const char* nmodl_filename = "/home/diogo/Dropbox/Diogo/H01_Network_model/cod/GPU_test/Human_L23_NetPyNE-main/sim/mod/NMDA.mod";
    const char* nmodl_file_text = 
  "TITLE NMDA receptor with voltage-dependent Mg2+ block\n"
  "\n"
  "COMMENT\n"
  "NMDA receptor conductance with a dual-exponential kinetic profile.\n"
  "\n"
  "\n"
  "The synaptic conductance is described by the difference between\n"
  "two state variables with rise and decay time constants.\n"
  "\n"
  "The NMDA receptor includes a voltage-dependent Mg2+ block\n"
  "following the formulation of Jahr and Stevens (1990).\n"
  "\n"
  "The original kinetic parameters and mathematical formulation\n"
  "are preserved from the original mechanism.\n"
  "\n"
  "No presynaptic short-term plasticity is implemented in this mechanism.\n"
  "\n"
  "\n"
  "ENDCOMMENT\n"
  "\n"
  "NEURON {\n"
  "POINT_PROCESS NMDA\n"
  "\n"
  "\n"
  "RANGE tau_r_NMDA\n"
  "RANGE tau_d_NMDA\n"
  "RANGE n_NMDA\n"
  "RANGE gama_NMDA\n"
  "\n"
  "RANGE i\n"
  "RANGE i_NMDA\n"
  "RANGE g_NMDA\n"
  "RANGE e\n"
  "\n"
  "NONSPECIFIC_CURRENT i\n"
  "\n"
  "\n"
  "}\n"
  "\n"
  "UNITS {\n"
  "(mV) = (millivolt)\n"
  "(nA) = (nanoamp)\n"
  "(uS) = (microsiemens)\n"
  "}\n"
  "\n"
  "PARAMETER {\n"
  "tau_r_NMDA = 0.3 (ms)\n"
  "tau_d_NMDA = 43 (ms)\n"
  "\n"
  "\n"
  "n_NMDA = 0.28011 (/mM)\n"
  "gama_NMDA = 0.062 (/mV)\n"
  "\n"
  "e = 0 (mV)\n"
  "\n"
  "mg = 1 (mM)\n"
  "\n"
  "\n"
  "}\n"
  "\n"
  "ASSIGNED {\n"
  "v (mV)\n"
  "\n"
  "\n"
  "i (nA)\n"
  "i_NMDA (nA)\n"
  "\n"
  "g_NMDA (uS)\n"
  "\n"
  "factor_NMDA\n"
  "mggate\n"
  "\n"
  "tp_NMDA (ms)\n"
  "\n"
  "\n"
  "}\n"
  "\n"
  "STATE {\n"
  "A_NMDA (uS)\n"
  "B_NMDA (uS)\n"
  "}\n"
  "\n"
  "INITIAL {\n"
  "A_NMDA = 0\n"
  "B_NMDA = 0\n"
  "\n"
  "\n"
  "tp_NMDA = (tau_r_NMDA * tau_d_NMDA) /\n"
  "          (tau_d_NMDA - tau_r_NMDA) *\n"
  "          log(tau_d_NMDA / tau_r_NMDA)\n"
  "\n"
  "factor_NMDA =\n"
  "    1 / (\n"
  "        -exp(-tp_NMDA / tau_r_NMDA) +\n"
  "         exp(-tp_NMDA / tau_d_NMDA)\n"
  "    )\n"
  "\n"
  "\n"
  "}\n"
  "\n"
  "BREAKPOINT {\n"
  "SOLVE state METHOD cnexp\n"
  "\n"
  "\n"
  "mggate =\n"
  "    1 / (\n"
  "        1 +\n"
  "        exp(gama_NMDA * (-v)) *\n"
  "        (mg / 3.57)\n"
  "    )\n"
  "\n"
  "g_NMDA =\n"
  "    (B_NMDA - A_NMDA) * mggate\n"
  "\n"
  "i_NMDA =\n"
  "    g_NMDA * (v - e)\n"
  "\n"
  "i =\n"
  "    i_NMDA\n"
  "\n"
  "\n"
  "}\n"
  "\n"
  "DERIVATIVE state {\n"
  "A_NMDA' =\n"
  "-A_NMDA / tau_r_NMDA\n"
  "\n"
  "\n"
  "B_NMDA' =\n"
  "    -B_NMDA / tau_d_NMDA\n"
  "\n"
  "\n"
  "}\n"
  "\n"
  "NET_RECEIVE(weight (uS)) {\n"
  "A_NMDA =\n"
  "A_NMDA +\n"
  "weight * factor_NMDA\n"
  "\n"
  "\n"
  "B_NMDA =\n"
  "    B_NMDA +\n"
  "    weight * factor_NMDA\n"
  "\n"
  "\n"
  "}\n"
  ;
    hoc_reg_nmodl_filename(mech_type, nmodl_filename);
    hoc_reg_nmodl_text(mech_type, nmodl_file_text);
}
#endif
