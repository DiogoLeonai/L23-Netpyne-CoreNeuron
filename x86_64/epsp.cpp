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
static constexpr auto number_of_floating_point_variables = 8;
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
 
#define nrn_init _nrn_init__epsp
#define _nrn_initial _nrn_initial__epsp
#define nrn_cur _nrn_cur__epsp
#define _nrn_current _nrn_current__epsp
#define nrn_jacob _nrn_jacob__epsp
#define nrn_state _nrn_state__epsp
#define _net_receive _net_receive__epsp 
 
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
#define onset _ml->template fpfield<0>(_iml)
#define onset_columnindex 0
#define tau0 _ml->template fpfield<1>(_iml)
#define tau0_columnindex 1
#define tau1 _ml->template fpfield<2>(_iml)
#define tau1_columnindex 2
#define imax _ml->template fpfield<3>(_iml)
#define imax_columnindex 3
#define i _ml->template fpfield<4>(_iml)
#define i_columnindex 4
#define myv _ml->template fpfield<5>(_iml)
#define myv_columnindex 5
#define v _ml->template fpfield<6>(_iml)
#define v_columnindex 6
#define _g _ml->template fpfield<7>(_iml)
#define _g_columnindex 7
#define _nd_area *_ml->dptr_field<0>(_iml)
 /* Thread safe. No static _ml, _iml or _ppvar. */
 static int hoc_nrnpointerindex =  -1;
 static _nrn_mechanism_std_vector<Datum> _extcall_thread;
 /* external NEURON variables */
 /* declaration of user functions */
 static double _hoc_curr(void*);
 static double _hoc_myexp(void*);
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
 {"curr", _hoc_curr},
 {"myexp", _hoc_myexp},
 {0, 0}
};
#define curr curr_epsp
#define myexp myexp_epsp
 extern double curr( _internalthreadargsprotocomma_ double );
 extern double myexp( _internalthreadargsprotocomma_ double );
 #define _za (_thread[0].get<double*>() + 0)
 #define _ztpeak _thread[0].get<double*>()[2]
 #define _zadjust _thread[0].get<double*>()[3]
 #define _zamp _thread[0].get<double*>()[4]
 /* declare global and static user variables */
 #define gind 0
 #define _gth 0
 /* some parameters have upper and lower limits */
 static HocParmLimits _hoc_parm_limits[] = {
 {0, 0, 0}
};
 static HocParmUnits _hoc_parm_units[] = {
 {"onset", "ms"},
 {"tau0", "ms"},
 {"tau1", "ms"},
 {"imax", "nA"},
 {"i", "nA"},
 {"myv", "mV"},
 {0, 0}
};
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
 /* connect range variables in _p that hoc is supposed to know about */
 static const char *_mechanism[] = {
 "7.7.0",
"epsp",
 "onset",
 "tau0",
 "tau1",
 "imax",
 0,
 "i",
 "myv",
 0,
 0,
 0};
 
 /* Used by NrnProperty */
 static _nrn_mechanism_std_vector<double> _parm_default{
     0, /* onset */
     0.2, /* tau0 */
     3, /* tau1 */
     0, /* imax */
 }; 
 
 
extern Prop* need_memb(Symbol*);
static void nrn_alloc(Prop* _prop) {
  Prop *prop_ion{};
  Datum *_ppvar{};
  if (nrn_point_prop_) {
    _nrn_mechanism_access_alloc_seq(_prop) = _nrn_mechanism_access_alloc_seq(nrn_point_prop_);
    _ppvar = _nrn_mechanism_access_dparam(nrn_point_prop_);
  } else {
   _ppvar = nrn_prop_datum_alloc(_mechtype, 2, _prop);
    _nrn_mechanism_access_dparam(_prop) = _ppvar;
     _nrn_mechanism_cache_instance _ml_real{_prop};
    auto* const _ml = &_ml_real;
    size_t const _iml{};
    assert(_nrn_mechanism_get_num_vars(_prop) == 8);
 	/*initialize range parameters*/
 	onset = _parm_default[0]; /* 0 */
 	tau0 = _parm_default[1]; /* 0.2 */
 	tau1 = _parm_default[2]; /* 3 */
 	imax = _parm_default[3]; /* 0 */
  }
 	 assert(_nrn_mechanism_get_num_vars(_prop) == 8);
 	_nrn_mechanism_access_dparam(_prop) = _ppvar;
 	/*connect ionic variables to this model*/
 
}
 static void _initlists();
 static void _thread_mem_init(Datum*);
 static void _thread_cleanup(Datum*);
 extern Symbol* hoc_lookup(const char*);
extern void _nrn_thread_reg(int, int, void(*)(Datum*));
void _nrn_thread_table_reg(int, nrn_thread_table_check_t);
extern void hoc_register_tolerance(int, HocStateTolerance*, Symbol***);
extern void _cvode_abstol( Symbol**, double*, int);

 extern "C" void _epsp_reg() {
	int _vectorized = 1;
  _initlists();
 	_pointtype = point_register_mech(_mechanism,
	 nrn_alloc,nrn_cur, nrn_jacob, nrn_state, nrn_init,
	 hoc_nrnpointerindex, 2,
	 _hoc_create_pnt, _hoc_destroy_pnt, _member_func);
  _extcall_thread.resize(1);
  _thread_mem_init(_extcall_thread.data());
 _mechtype = nrn_get_mechtype(_mechanism[1]);
 hoc_register_parm_default(_mechtype, &_parm_default);
     _nrn_setdata_reg(_mechtype, _setdata);
     _nrn_thread_reg(_mechtype, 1, _thread_mem_init);
     _nrn_thread_reg(_mechtype, 0, _thread_cleanup);
 #if NMODL_TEXT
  register_nmodl_text_and_filename(_mechtype);
#endif
   _nrn_mechanism_register_data_fields(_mechtype,
                                       _nrn_mechanism_field<double>{"onset"} /* 0 */,
                                       _nrn_mechanism_field<double>{"tau0"} /* 1 */,
                                       _nrn_mechanism_field<double>{"tau1"} /* 2 */,
                                       _nrn_mechanism_field<double>{"imax"} /* 3 */,
                                       _nrn_mechanism_field<double>{"i"} /* 4 */,
                                       _nrn_mechanism_field<double>{"myv"} /* 5 */,
                                       _nrn_mechanism_field<double>{"v"} /* 6 */,
                                       _nrn_mechanism_field<double>{"_g"} /* 7 */,
                                       _nrn_mechanism_field<double*>{"_nd_area", "area"} /* 0 */,
                                       _nrn_mechanism_field<Point_process*>{"_pntproc", "pntproc"} /* 1 */);
  hoc_register_prop_size(_mechtype, 8, 2);
  hoc_register_dparam_semantics(_mechtype, 0, "area");
  hoc_register_dparam_semantics(_mechtype, 1, "pntproc");
 
    hoc_register_var(hoc_scdoub, hoc_vdoub, hoc_intfunc);
 	ivoc_help("help ?1 epsp /home/diogo/Dropbox/Diogo/H01_Network_model/cod/Human_L23_NetPyNE-main/sim/mod/epsp.mod\n");
 hoc_register_limits(_mechtype, _hoc_parm_limits);
 hoc_register_units(_mechtype, _hoc_parm_units);
 }
 /*Top LOCAL _za [ 2 ] */
 /*Top LOCAL _ztpeak */
 /*Top LOCAL _zadjust */
 /*Top LOCAL _zamp */
static int _reset;
static const char *modelname = "";

static int error;
static int _ninits = 0;
static int _match_recurse=1;
static void _modl_cleanup(){ _match_recurse=1;}
 
double myexp ( _internalthreadargsprotocomma_ double _lx ) {
   double _lmyexp;
 if ( _lx < - 100.0 ) {
     _lmyexp = 0.0 ;
     }
   else {
     _lmyexp = exp ( _lx ) ;
     }
   
return _lmyexp;
 }
 
static double _hoc_myexp(void* _vptr) {
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
 _r =  myexp ( _threadargscomma_ *getarg(1) );
 return(_r);
}
 
double curr ( _internalthreadargsprotocomma_ double _lx ) {
   double _lcurr;
 _ztpeak = tau0 * tau1 * log ( tau0 / tau1 ) / ( tau0 - tau1 ) ;
   _zadjust = 1.0 / ( ( 1.0 - myexp ( _threadargscomma_ - _ztpeak / tau0 ) ) - ( 1.0 - myexp ( _threadargscomma_ - _ztpeak / tau1 ) ) ) ;
   _zamp = _zadjust * imax ;
   if ( _lx < onset ) {
     _lcurr = 0.0 ;
     }
   else {
     _za [ 0 ] = 1.0 - myexp ( _threadargscomma_ - ( _lx - onset ) / tau0 ) ;
     _za [ 1 ] = 1.0 - myexp ( _threadargscomma_ - ( _lx - onset ) / tau1 ) ;
     _lcurr = - _zamp * ( _za [ 0 ] - _za [ 1 ] ) ;
     }
   
return _lcurr;
 }
 
static double _hoc_curr(void* _vptr) {
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
 _r =  curr ( _threadargscomma_ *getarg(1) );
 return(_r);
}
 
static void _thread_mem_init(Datum* _thread) {
   _thread[0] = {neuron::container::do_not_search, new double[5]{}};
 }
 
static void _thread_cleanup(Datum* _thread) {
   delete[] _thread[0].get<double*>();
 }

static void initmodel(_internalthreadargsproto_) {
  int _i; double _save;{

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
   myv = v ;
   i = curr ( _threadargscomma_ t ) ;
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

}

static void terminal(){}

static void _initlists(){
 int _i; static int _first = 1;
  if (!_first) return;
_first = 0;
}

#if NMODL_TEXT
static void register_nmodl_text_and_filename(int mech_type) {
    const char* nmodl_filename = "/home/diogo/Dropbox/Diogo/H01_Network_model/cod/Human_L23_NetPyNE-main/sim/mod/epsp.mod";
    const char* nmodl_file_text = 
  ": this model is built-in to neuron with suffix epsp\n"
  ": Schaefer et al. 2003\n"
  "\n"
  "COMMENT\n"
  "modified from syn2.mod\n"
  "injected current with exponential rise and decay current defined by\n"
  "         i = 0 for t < onset and\n"
  "         i=amp*((1-exp(-(t-onset)/tau0))-(1-exp(-(t-onset)/tau1)))\n"
  "          for t > onset\n"
  "\n"
  "	compare to experimental current injection:\n"
  " 	i = - amp*(1-exp(-t/t1))*(exp(-t/t2))\n"
  "\n"
  "	-> tau1==t2   tau0 ^-1 = t1^-1 + t2^-1\n"
  "ENDCOMMENT\n"
  "					       \n"
  "INDEPENDENT {t FROM 0 TO 1 WITH 1 (ms)}\n"
  "\n"
  "NEURON {\n"
  "	POINT_PROCESS epsp\n"
  "	RANGE onset, tau0, tau1, imax, i, myv\n"
  "	NONSPECIFIC_CURRENT i\n"
  "}\n"
  "UNITS {\n"
  "	(nA) = (nanoamp)\n"
  "	(mV) = (millivolt)\n"
  "	(umho) = (micromho)\n"
  "}\n"
  "\n"
  "PARAMETER {\n"
  "	onset=0  (ms)\n"
  "	tau0=0.2 (ms)\n"
  "	tau1=3.0 (ms)\n"
  "	imax=0 	 (nA)\n"
  "	v	 (mV)\n"
  "}\n"
  "\n"
  "ASSIGNED { i (nA)  myv (mV)}\n"
  "\n"
  "LOCAL   a[2]\n"
  "LOCAL   tpeak\n"
  "LOCAL   adjust\n"
  "LOCAL   amp\n"
  "\n"
  "BREAKPOINT {\n"
  "	myv = v\n"
  "        i = curr(t)\n"
  "}\n"
  "\n"
  "FUNCTION myexp(x) {\n"
  "	if (x < -100) {\n"
  "	myexp = 0\n"
  "	}else{\n"
  "	myexp = exp(x)\n"
  "	}\n"
  "}\n"
  "\n"
  "FUNCTION curr(x) {				\n"
  "	tpeak=tau0*tau1*log(tau0/tau1)/(tau0-tau1)\n"
  "	adjust=1/((1-myexp(-tpeak/tau0))-(1-myexp(-tpeak/tau1)))\n"
  "	amp=adjust*imax\n"
  "	if (x < onset) {\n"
  "		curr = 0\n"
  "	}else{\n"
  "		a[0]=1-myexp(-(x-onset)/tau0)\n"
  "		a[1]=1-myexp(-(x-onset)/tau1)\n"
  "		curr = -amp*(a[0]-a[1])\n"
  "	}\n"
  "}\n"
  ;
    hoc_reg_nmodl_filename(mech_type, nmodl_filename);
    hoc_reg_nmodl_text(mech_type, nmodl_file_text);
}
#endif
