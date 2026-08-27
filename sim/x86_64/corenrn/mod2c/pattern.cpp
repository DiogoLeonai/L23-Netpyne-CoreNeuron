/*********************************************************
Model Name      : PatternStim
Filename        : pattern.mod
NMODL Version   : 7.7.0
Vectorized      : true
Threadsafe      : true
Created         : Thu Aug 27 09:07:35 2026
Simulator       : CoreNEURON
Backend         : C++-OpenAcc (api-compatibility)
NMODL Compiler  : 9.0.1 []
*********************************************************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <coreneuron/utils/offload.hpp>
#include <cuda_runtime_api.h>

#include <coreneuron/gpu/nrn_acc_manager.hpp>
#include <coreneuron/mechanism/mech/mod2c_core_thread.hpp>
#include <coreneuron/mechanism/register_mech.hpp>
#include <coreneuron/nrnconf.h>
#include <coreneuron/nrniv/nrniv_decl.h>
#include <coreneuron/sim/multicore.hpp>
#include <coreneuron/sim/scopmath/newton_thread.hpp>
#include <coreneuron/utils/ivocvect.hpp>
#include <coreneuron/utils/nrnoc_aux.hpp>
#include <coreneuron/utils/randoms/nrnran123.h>


namespace coreneuron {
    #ifndef NRN_PRCELLSTATE
    #define NRN_PRCELLSTATE 0
    #endif


    /** channel information */
    static const char *mechanism_info[] = {
        "7.7.0",
        "PatternStim",
        "fake_output",
        0,
        0,
        0,
        "ptr",
        0
    };


    /** all global variables */
    struct PatternStim_Store {
        int point_type{};
        int reset{};
        int mech_type{};
    };
    static_assert(std::is_trivially_copy_constructible_v<PatternStim_Store>);
    static_assert(std::is_trivially_move_constructible_v<PatternStim_Store>);
    static_assert(std::is_trivially_copy_assignable_v<PatternStim_Store>);
    static_assert(std::is_trivially_move_assignable_v<PatternStim_Store>);
    static_assert(std::is_trivially_destructible_v<PatternStim_Store>);
    static PatternStim_Store PatternStim_global;


    /** all mechanism instance variables and global variables */
    struct PatternStim_Instance  {
        double* fake_output{};
        double* v_unused{};
        double* tsave{};
        const double* node_area{};
        void** point_process{};
        void** ptr{};
        void** tqitem{};
        PatternStim_Store* global{&PatternStim_global};
    };


    /** connect global (scalar) variables to hoc -- */
    static DoubScal hoc_scalar_double[] = {
        {nullptr, nullptr}
    };


    /** connect global (array) variables to hoc -- */
    static DoubVec hoc_vector_double[] = {
        {nullptr, nullptr, 0}
    };


    static inline int first_pointer_var_index() {
        return 2;
    }


    static inline int first_random_var_index() {
        return -1;
    }


    static inline int num_net_receive_args() {
        return 1;
    }


    static inline int float_variables_size() {
        return 3;
    }


    static inline int int_variables_size() {
        return 4;
    }


    static inline int get_mech_type() {
        return PatternStim_global.mech_type;
    }


    static inline Memb_list* get_memb_list(NrnThread* nt) {
        if (!nt->_ml_list) {
            return nullptr;
        }
        return nt->_ml_list[get_mech_type()];
    }


    static inline void* mem_alloc(size_t num, size_t size, size_t alignment = 64) {
        size_t aligned_size = ((num*size + alignment - 1) / alignment) * alignment;
        void* ptr = aligned_alloc(alignment, aligned_size);
        memset(ptr, 0, aligned_size);
        return ptr;
    }


    static inline void mem_free(void* ptr) {
        free(ptr);
    }


    static inline void coreneuron_abort() {
        printf("Error : Issue while running OpenACC kernel \n");
        assert(0==1);
    }

    // Allocate instance structure
    static void nrn_private_constructor_PatternStim(NrnThread* nt, Memb_list* ml, int type) {
        assert(!ml->instance);
        assert(!ml->global_variables);
        assert(ml->global_variables_size == 0);
        auto* const inst = new PatternStim_Instance{};
        assert(inst->global == &PatternStim_global);
        ml->instance = inst;
        ml->global_variables = inst->global;
        ml->global_variables_size = sizeof(PatternStim_Store);
    }

    // Deallocate the instance structure
    static void nrn_private_destructor_PatternStim(NrnThread* nt, Memb_list* ml, int type) {
        auto* const inst = static_cast<PatternStim_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &PatternStim_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(PatternStim_Store));
        delete inst;
        ml->instance = nullptr;
        ml->global_variables = nullptr;
        ml->global_variables_size = 0;
    }

    /** initialize mechanism instance variables */
    static inline void setup_instance(NrnThread* nt, Memb_list* ml) {
        auto* const inst = static_cast<PatternStim_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &PatternStim_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(PatternStim_Store));
        int pnodecount = ml->_nodecount_padded;
        Datum* indexes = ml->pdata;
        inst->fake_output = ml->data+0*pnodecount;
        inst->v_unused = ml->data+1*pnodecount;
        inst->tsave = ml->data+2*pnodecount;
        inst->node_area = nt->_data;
        inst->point_process = nt->_vdata;
        inst->ptr = nt->_vdata;
        inst->tqitem = nt->_vdata;
    }



    static void nrn_alloc_PatternStim(double* data, Datum* indexes, int type) {
        // do nothing
    }


    void nrn_constructor_PatternStim(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<PatternStim_Instance*>(ml->instance);

        #endif
    }


    void nrn_destructor_PatternStim(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<PatternStim_Instance*>(ml->instance);

        #endif
    }


    inline static double initps_PatternStim(int id, int pnodecount, PatternStim_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v);
    inline static double sendgroup_PatternStim(int id, int pnodecount, PatternStim_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v);
}


using namespace coreneuron;


// VERBATIM
struct Info {
	int size;
	double* tvec;
	int* gidvec;
	int index;
};
#define INFOCAST Info** ip = (Info**)(&(nt->_vdata[indexes[2*pnodecount + id]]))
// ENDVERBATIM


// VERBATIM
Info* mkinfo(int id, int pnodecount, double* data, Datum* indexes, ThreadDatum* thread, NrnThread* nt, Memb_list* ml, double v) {
	INFOCAST;
	Info* info = (Info*)hoc_Emalloc(sizeof(Info)); hoc_malchk();
	info->size = 0;
	info->tvec = nullptr;
	info->gidvec = nullptr;
	info->index = 0;
	return info;
}
/* for CoreNEURON checkpoint save and restore */
namespace coreneuron {
int checkpoint_save_patternstim(int id, int pnodecount, double* data, Datum* indexes, ThreadDatum* thread, NrnThread* nt, Memb_list* ml, double v) {
	INFOCAST; Info* info = *ip;
	return info->index;
}
void checkpoint_restore_patternstim(int _index, double _te, int id, int pnodecount, double* data, Datum* indexes, ThreadDatum* thread, NrnThread* nt, Memb_list* ml, double v) {
    INFOCAST; Info* info = *ip;
    info->index = _index;
    artcell_net_send(&nt->_vdata[indexes[3*pnodecount + id]], -1, (Point_process*)nt->_vdata[indexes[1*pnodecount+id]], _te, 1.0);
}
} 
// ENDVERBATIM


// VERBATIM
static void bbcore_write(double* x, int* d, int* xx, int *offset, int id, int pnodecount, double* data, Datum* indexes, ThreadDatum* thread, NrnThread* nt, Memb_list* ml, double v){}
static void bbcore_read(double* x, int* d, int* xx, int* offset, int id, int pnodecount, double* data, Datum* indexes, ThreadDatum* thread, NrnThread* nt, Memb_list* ml, double v){}
namespace coreneuron {
void pattern_stim_setup_helper(int size, double* tv, int* gv, int id, int pnodecount, double* data, Datum* indexes, ThreadDatum* thread, NrnThread* nt, Memb_list* ml, double v) {
	INFOCAST;
	Info* info = mkinfo(id, pnodecount, data, indexes, thread, nt, ml, v);
	*ip = info;
	info->size = size;
	info->tvec = tv;
	info->gidvec = gv;
	artcell_net_send ( &nt->_vdata[indexes[3*pnodecount + id]], -1, (Point_process*) nt->_vdata[indexes[1*pnodecount+id]], nt->_t +  0.0 , 1.0 ) ;
}
Info** pattern_stim_info_ref(int id, int pnodecount, double* data, Datum* indexes, ThreadDatum* thread, NrnThread* nt, Memb_list* ml, double v) {
    INFOCAST;
    return ip; 
}
} 
// ENDVERBATIM


namespace coreneuron {


    inline double initps_PatternStim(int id, int pnodecount, PatternStim_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        double ret_initps = 0.0;
        // VERBATIM
         {
        	INFOCAST; Info* info = *ip;
        	info->index = 0;
        	if (info && info->tvec) {
        		ret_initps = 1.;
        	}else{
        		ret_initps = 0.;
        	}
        }
        // ENDVERBATIM

        return ret_initps;
    }


    inline double sendgroup_PatternStim(int id, int pnodecount, PatternStim_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        double ret_sendgroup = 0.0;
        // VERBATIM
         {
        	INFOCAST; Info* info = *ip;
        	int size = info->size;
        	int fake_out;
        	double* tvec = info->tvec;
        	int* gidvec = info->gidvec;
        	int i;
        	fake_out = inst->fake_output[id] ? 1 : 0;
        	for (i=0; info->index < size; ++i) {
        		/* only if the gid is NOT on this machine */
        		nrn_fake_fire(gidvec[info->index], tvec[info->index], fake_out);
        		++info->index;
        		if (i > 100 && nt->_t < tvec[info->index]) { break; }
        	}
        	if (info->index >= size) {
        		ret_sendgroup = nt->_t - 1.;
        	}else{
        		ret_sendgroup = tvec[info->index];
        	}
        }
        // ENDVERBATIM

        return ret_sendgroup;
    }


    static inline void net_receive_PatternStim(Point_process* pnt, int weight_index, double flag) {
        int tid = pnt->_tid;
        int id = pnt->_i_instance;
        double v = 0;
        NrnThread* nt = nrn_threads + tid;
        Memb_list* ml = nt->_ml_list[pnt->_type];
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        double* data = ml->data;
        double* weights = nt->weights;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<PatternStim_Instance*>(ml->instance);

        double t = nt->_t;
        inst->tsave[id] = t;
        {
            double nst;
            if (flag == 1.0) {
                double sendgroup_in_0;
                {
                    // VERBATIM
                     {
                    	INFOCAST; Info* info = *ip;
                    	int size = info->size;
                    	int fake_out;
                    	double* tvec = info->tvec;
                    	int* gidvec = info->gidvec;
                    	int i;
                    	fake_out = inst->fake_output[id] ? 1 : 0;
                    	for (i=0; info->index < size; ++i) {
                    		/* only if the gid is NOT on this machine */
                    		nrn_fake_fire(gidvec[info->index], tvec[info->index], fake_out);
                    		++info->index;
                    		if (i > 100 && t < tvec[info->index]) { break; }
                    	}
                    	if (info->index >= size) {
                    		sendgroup_in_0 = t - 1.;
                    	}else{
                    		sendgroup_in_0 = tvec[info->index];
                    	}
                    }
                    // ENDVERBATIM

                }
                nst = sendgroup_in_0;
                if (nst >= t) {
                    artcell_net_send(&inst->tqitem[indexes[3*pnodecount + id]], weight_index, pnt, nt->_t+nst - t, 1.0);
                }
            }
        }
    }


    /** initialize channel */
    void nrn_init_PatternStim(NrnThread* nt, Memb_list* ml, int type) {
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;

        setup_instance(nt, ml);
        auto* const inst = static_cast<PatternStim_Instance*>(ml->instance);

        if (_nrn_skip_initmodel == 0) {
            for (int id = 0; id < nodecount; id++) {
                inst->tsave[id] = -1e20;
                double v = 0.0;
                double initps_in_0;
                {
                    // VERBATIM
                     {
                    	INFOCAST; Info* info = *ip;
                    	info->index = 0;
                    	if (info && info->tvec) {
                    		initps_in_0 = 1.;
                    	}else{
                    		initps_in_0 = 0.;
                    	}
                    }
                    // ENDVERBATIM

                }
                if (initps_in_0 > 0.0) {
                    artcell_net_send(&inst->tqitem[indexes[3*pnodecount + id]], 0, (Point_process*)inst->point_process[indexes[1*pnodecount + id]], nt->_t+0.0, 1.0);
                }
            }
        }
    }


    /** register channel with the simulator */
    void _pattern_reg() {

        int mech_type = nrn_get_mechtype("PatternStim");
        PatternStim_global.mech_type = mech_type;
        if (mech_type == -1) {
            return;
        }

        _nrn_layout_reg(mech_type, 0);
        point_register_mech(mechanism_info, nrn_alloc_PatternStim, nullptr, nullptr, nullptr, nrn_init_PatternStim, nrn_private_constructor_PatternStim, nrn_private_destructor_PatternStim, first_pointer_var_index(), nullptr, nullptr, 1);

        hoc_reg_bbcore_read(mech_type, bbcore_read);
        hoc_reg_bbcore_write(mech_type, bbcore_write);
        hoc_register_prop_size(mech_type, float_variables_size(), int_variables_size());
        hoc_register_dparam_semantics(mech_type, 0, "area");
        hoc_register_dparam_semantics(mech_type, 1, "pntproc");
        hoc_register_dparam_semantics(mech_type, 2, "bbcorepointer");
        hoc_register_dparam_semantics(mech_type, 3, "netsend");
        add_nrn_artcell(mech_type, 3);
        set_pnt_receive(mech_type, net_receive_PatternStim, nullptr, num_net_receive_args());
        hoc_register_net_send_buffering(mech_type);
        hoc_register_var(hoc_scalar_double, hoc_vector_double, NULL);
    }
}
