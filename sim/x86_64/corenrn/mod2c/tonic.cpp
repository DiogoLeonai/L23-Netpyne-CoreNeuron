/*********************************************************
Model Name      : tonic
Filename        : tonic.mod
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
        "tonic",
        "g_tonic",
        "e_gaba_tonic",
        0,
        "i_tonic",
        "a_tonic",
        "b_tonic",
        "c_tonic",
        0,
        "o_tonic",
        0,
        0
    };


    /** all global variables */
    struct tonic_Store {
        double o0{};
        int reset{};
        int mech_type{};
        int slist1[1]{6};
        int dlist1[1]{7};
    };
    static_assert(std::is_trivially_copy_constructible_v<tonic_Store>);
    static_assert(std::is_trivially_move_constructible_v<tonic_Store>);
    static_assert(std::is_trivially_copy_assignable_v<tonic_Store>);
    static_assert(std::is_trivially_move_assignable_v<tonic_Store>);
    static_assert(std::is_trivially_destructible_v<tonic_Store>);
    static tonic_Store tonic_global;


    /** all mechanism instance variables and global variables */
    struct tonic_Instance  {
        const double* g{};
        const double* e_gaba{};
        double* i{};
        double* a{};
        double* b{};
        double* c{};
        double* o{};
        double* Do{};
        double* v_unused{};
        double* g_unused{};
        tonic_Store* global{&tonic_global};
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
        return -1;
    }


    static inline int first_random_var_index() {
        return -1;
    }


    static inline int float_variables_size() {
        return 10;
    }


    static inline int int_variables_size() {
        return 0;
    }


    static inline int get_mech_type() {
        return tonic_global.mech_type;
    }


    static inline Memb_list* get_memb_list(NrnThread* nt) {
        if (!nt->_ml_list) {
            return nullptr;
        }
        return nt->_ml_list[get_mech_type()];
    }


    static inline void* mem_alloc(size_t num, size_t size, size_t alignment = 16) {
        void* ptr;
        cudaMallocManaged(&ptr, num*size);
        cudaMemset(ptr, 0, num*size);
        return ptr;
    }


    static inline void mem_free(void* ptr) {
        cudaFree(ptr);
    }


    static inline void coreneuron_abort() {
        printf("Error : Issue while running OpenACC kernel \n");
        assert(0==1);
    }

    // Allocate instance structure
    static void nrn_private_constructor_tonic(NrnThread* nt, Memb_list* ml, int type) {
        assert(!ml->instance);
        assert(!ml->global_variables);
        assert(ml->global_variables_size == 0);
        auto* const inst = new tonic_Instance{};
        assert(inst->global == &tonic_global);
        ml->instance = inst;
        ml->global_variables = inst->global;
        ml->global_variables_size = sizeof(tonic_Store);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, tonic_Instance const* inst);
    static inline void delete_instance_from_device(tonic_Instance* inst);
    // Deallocate the instance structure
    static void nrn_private_destructor_tonic(NrnThread* nt, Memb_list* ml, int type) {
        auto* const inst = static_cast<tonic_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &tonic_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(tonic_Store));
        delete_instance_from_device(inst);
        delete inst;
        ml->instance = nullptr;
        ml->global_variables = nullptr;
        ml->global_variables_size = 0;
    }

    /** initialize mechanism instance variables */
    static inline void setup_instance(NrnThread* nt, Memb_list* ml) {
        auto* const inst = static_cast<tonic_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &tonic_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(tonic_Store));
        int pnodecount = ml->_nodecount_padded;
        Datum* indexes = ml->pdata;
        inst->g = ml->data+0*pnodecount;
        inst->e_gaba = ml->data+1*pnodecount;
        inst->i = ml->data+2*pnodecount;
        inst->a = ml->data+3*pnodecount;
        inst->b = ml->data+4*pnodecount;
        inst->c = ml->data+5*pnodecount;
        inst->o = ml->data+6*pnodecount;
        inst->Do = ml->data+7*pnodecount;
        inst->v_unused = ml->data+8*pnodecount;
        inst->g_unused = ml->data+9*pnodecount;
        copy_instance_to_device(nt, ml, inst);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, tonic_Instance const* inst) {
        if (!nt->compute_gpu) {
            return;
        }
        auto tmp = *inst;
        auto* d_inst = cnrn_target_is_present(inst);
        if (!d_inst) {
            d_inst = cnrn_target_copyin(inst);
        }
        tmp.global = cnrn_target_deviceptr(tmp.global);
        tmp.g = cnrn_target_deviceptr(tmp.g);
        tmp.e_gaba = cnrn_target_deviceptr(tmp.e_gaba);
        tmp.i = cnrn_target_deviceptr(tmp.i);
        tmp.a = cnrn_target_deviceptr(tmp.a);
        tmp.b = cnrn_target_deviceptr(tmp.b);
        tmp.c = cnrn_target_deviceptr(tmp.c);
        tmp.o = cnrn_target_deviceptr(tmp.o);
        tmp.Do = cnrn_target_deviceptr(tmp.Do);
        tmp.v_unused = cnrn_target_deviceptr(tmp.v_unused);
        tmp.g_unused = cnrn_target_deviceptr(tmp.g_unused);
        cnrn_target_memcpy_to_device(d_inst, &tmp);
        auto* d_ml = cnrn_target_deviceptr(ml);
        void* d_inst_void = d_inst;
        cnrn_target_memcpy_to_device(&(d_ml->instance), &d_inst_void);
    }

    static inline void delete_instance_from_device(tonic_Instance* inst) {
        if (cnrn_target_is_present(inst)) {
            cnrn_target_delete(inst);
        }
    }



    static void nrn_alloc_tonic(double* data, Datum* indexes, int type) {
        // do nothing
    }


    void nrn_constructor_tonic(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<tonic_Instance*>(ml->instance);

        #endif
    }


    void nrn_destructor_tonic(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<tonic_Instance*>(ml->instance);

        #endif
    }


    inline static int rates_tonic(int id, int pnodecount, tonic_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v);


    inline int rates_tonic(int id, int pnodecount, tonic_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        int ret_rates = 0;
        double x, y;
        x = 0.1 * (v + 20.0);
        if (fabs(x) > 1e-6) {
            inst->a[id] = (50.0 * x) / (1.0 - exp( -x));
        } else {
            inst->a[id] = 0.25 * (pow(v, 2.0) + 20.0 * v + 200.0);
        }
        y =  -0.08 * (v - 10.0);
        if (fabs(y) > 1e-6) {
            inst->b[id] = (20.0 * y) / (1.0 - exp( -y));
        } else {
            inst->b[id] =  -0.064 * (pow(v, 2.0) - 45.0 * v + 37.5);
        }
        return ret_rates;
    }


    /** initialize channel */
    void nrn_init_tonic(NrnThread* nt, Memb_list* ml, int type) {
        nrn_pragma_acc(data present(nt, ml) if(nt->compute_gpu))
        {
            int nodecount = ml->nodecount;
            int pnodecount = ml->_nodecount_padded;
            const int* node_index = ml->nodeindices;
            double* data = ml->data;
            const double* voltage = nt->_actual_v;
            Datum* indexes = ml->pdata;
            ThreadDatum* thread = ml->_thread;

            setup_instance(nt, ml);
            auto* const inst = static_cast<tonic_Instance*>(ml->instance);

            if (nt->compute_gpu) {
                nrn_pragma_acc(update device (tonic_global))
                nrn_pragma_omp(target update to(tonic_global))
            }
            if (_nrn_skip_initmodel == 0) {
                nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread) async(nt->stream_id) if(nt->compute_gpu))
                nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
                for (int id = 0; id < nodecount; id++) {
                    int node_id = node_index[id];
                    double v = voltage[node_id];
                    #if NRN_PRCELLSTATE
                    inst->v_unused[id] = v;
                    #endif
                    inst->o[id] = inst->global->o0;
                    {
                        double x, y;
                        x = 0.1 * (v + 20.0);
                        if (fabs(x) > 1e-6) {
                            inst->a[id] = (50.0 * x) / (1.0 - exp( -x));
                        } else {
                            inst->a[id] = 0.25 * (pow(v, 2.0) + 20.0 * v + 200.0);
                        }
                        y =  -0.08 * (v - 10.0);
                        if (fabs(y) > 1e-6) {
                            inst->b[id] = (20.0 * y) / (1.0 - exp( -y));
                        } else {
                            inst->b[id] =  -0.064 * (pow(v, 2.0) - 45.0 * v + 37.5);
                        }
                    }
                    inst->o[id] = inst->a[id] / (inst->a[id] + inst->b[id]);
                    inst->c[id] = 1.0 - inst->o[id];
                }
            }
        }
    }


    inline double nrn_current_tonic(int id, int pnodecount, tonic_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        double current = 0.0;
        inst->c[id] = 1.0 - inst->o[id];
        inst->i[id] = inst->g[id] * inst->o[id] * (v - inst->e_gaba[id]);
        current += inst->i[id];
        return current;
    }


    /** update current */
    void nrn_cur_tonic(NrnThread* nt, Memb_list* ml, int type) {
        nrn_pragma_acc(data present(nt, ml) if(nt->compute_gpu))
        {
            int nodecount = ml->nodecount;
            int pnodecount = ml->_nodecount_padded;
            const int* node_index = ml->nodeindices;
            double* data = ml->data;
            const double* voltage = nt->_actual_v;
            double* vec_rhs = nt->_actual_rhs;
            double* vec_d = nt->_actual_d;
            Datum* indexes = ml->pdata;
            ThreadDatum* thread = ml->_thread;
            auto* const inst = static_cast<tonic_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread, vec_rhs, vec_d) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                double g = nrn_current_tonic(id, pnodecount, inst, data, indexes, thread, nt, v+0.001);
                double rhs = nrn_current_tonic(id, pnodecount, inst, data, indexes, thread, nt, v);
                g = (g-rhs)/0.001;
                #if NRN_PRCELLSTATE
                inst->g_unused[id] = g;
                #endif
                vec_rhs[node_id] -= rhs;
                vec_d[node_id] += g;
            }
        }
    }


    /** update state */
    void nrn_state_tonic(NrnThread* nt, Memb_list* ml, int type) {
        nrn_pragma_acc(data present(nt, ml) if(nt->compute_gpu))
        {
            int nodecount = ml->nodecount;
            int pnodecount = ml->_nodecount_padded;
            const int* node_index = ml->nodeindices;
            double* data = ml->data;
            const double* voltage = nt->_actual_v;
            Datum* indexes = ml->pdata;
            ThreadDatum* thread = ml->_thread;
            auto* const inst = static_cast<tonic_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                {
                    double x, y;
                    x = 0.1 * (v + 20.0);
                    if (fabs(x) > 1e-6) {
                        inst->a[id] = (50.0 * x) / (1.0 - exp( -x));
                    } else {
                        inst->a[id] = 0.25 * (pow(v, 2.0) + 20.0 * v + 200.0);
                    }
                    y =  -0.08 * (v - 10.0);
                    if (fabs(y) > 1e-6) {
                        inst->b[id] = (20.0 * y) / (1.0 - exp( -y));
                    } else {
                        inst->b[id] =  -0.064 * (pow(v, 2.0) - 45.0 * v + 37.5);
                    }
                }
                inst->o[id] = inst->o[id] + (1.0 - exp(nt->_dt * ((inst->a[id]) * ((( -1.0))) - (inst->b[id]) * (1.0)))) * ( -((inst->a[id]) * ((1.0))) / ((inst->a[id]) * ((( -1.0))) - (inst->b[id]) * (1.0)) - inst->o[id]);
            }
        }
    }


    /** register channel with the simulator */
    void _tonic_reg() {

        int mech_type = nrn_get_mechtype("tonic");
        tonic_global.mech_type = mech_type;
        if (mech_type == -1) {
            return;
        }

        _nrn_layout_reg(mech_type, 0);
        register_mech(mechanism_info, nrn_alloc_tonic, nrn_cur_tonic, nullptr, nrn_state_tonic, nrn_init_tonic, nrn_private_constructor_tonic, nrn_private_destructor_tonic, first_pointer_var_index(), 1);

        hoc_register_prop_size(mech_type, float_variables_size(), int_variables_size());
        hoc_register_var(hoc_scalar_double, hoc_vector_double, NULL);
    }
}
