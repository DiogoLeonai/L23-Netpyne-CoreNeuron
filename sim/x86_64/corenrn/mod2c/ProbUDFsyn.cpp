/*********************************************************
Model Name      : ProbUDFsyn
Filename        : ProbUDFsyn.mod
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
        0
    };


    /** all global variables */
    struct ProbUDFsyn_Store {
        int point_type{};
        double A0{};
        double B0{};
        int reset{};
        int mech_type{};
        int slist1[2]{10, 11};
        int dlist1[2]{13, 14};
    };
    static_assert(std::is_trivially_copy_constructible_v<ProbUDFsyn_Store>);
    static_assert(std::is_trivially_move_constructible_v<ProbUDFsyn_Store>);
    static_assert(std::is_trivially_copy_assignable_v<ProbUDFsyn_Store>);
    static_assert(std::is_trivially_move_assignable_v<ProbUDFsyn_Store>);
    static_assert(std::is_trivially_destructible_v<ProbUDFsyn_Store>);
    static ProbUDFsyn_Store ProbUDFsyn_global;


    /** all mechanism instance variables and global variables */
    struct ProbUDFsyn_Instance  {
        const double* tau_r{};
        const double* tau_d{};
        const double* Use{};
        const double* Dep{};
        const double* Fac{};
        const double* u0{};
        const double* e{};
        const double* gmax{};
        double* i{};
        double* g{};
        double* A{};
        double* B{};
        double* factor{};
        double* DA{};
        double* DB{};
        double* v_unused{};
        double* g_unused{};
        double* tsave{};
        const double* node_area{};
        const int* point_process{};
        void** rng{};
        ProbUDFsyn_Store* global{&ProbUDFsyn_global};
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
        return 2;
    }


    static inline int num_net_receive_args() {
        return 5;
    }


    static inline int float_variables_size() {
        return 18;
    }


    static inline int int_variables_size() {
        return 3;
    }


    static inline int get_mech_type() {
        return ProbUDFsyn_global.mech_type;
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
    static void nrn_private_constructor_ProbUDFsyn(NrnThread* nt, Memb_list* ml, int type) {
        assert(!ml->instance);
        assert(!ml->global_variables);
        assert(ml->global_variables_size == 0);
        auto* const inst = new ProbUDFsyn_Instance{};
        assert(inst->global == &ProbUDFsyn_global);
        ml->instance = inst;
        ml->global_variables = inst->global;
        ml->global_variables_size = sizeof(ProbUDFsyn_Store);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, ProbUDFsyn_Instance const* inst);
    static inline void delete_instance_from_device(ProbUDFsyn_Instance* inst);
    // Deallocate the instance structure
    static void nrn_private_destructor_ProbUDFsyn(NrnThread* nt, Memb_list* ml, int type) {
        auto* const inst = static_cast<ProbUDFsyn_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &ProbUDFsyn_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(ProbUDFsyn_Store));
        int pnodecount = ml->_nodecount_padded;
        int nodecount = ml->nodecount;
        Datum* indexes = ml->pdata;
        for (int id = 0; id < nodecount; id++) {
            nrnran123_deletestream((nrnran123_State*) inst->rng[indexes[2*pnodecount + id]]);
        }
        delete_instance_from_device(inst);
        delete inst;
        ml->instance = nullptr;
        ml->global_variables = nullptr;
        ml->global_variables_size = 0;
    }

    /** initialize mechanism instance variables */
    static inline void setup_instance(NrnThread* nt, Memb_list* ml) {
        auto* const inst = static_cast<ProbUDFsyn_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &ProbUDFsyn_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(ProbUDFsyn_Store));
        int pnodecount = ml->_nodecount_padded;
        Datum* indexes = ml->pdata;
        inst->tau_r = ml->data+0*pnodecount;
        inst->tau_d = ml->data+1*pnodecount;
        inst->Use = ml->data+2*pnodecount;
        inst->Dep = ml->data+3*pnodecount;
        inst->Fac = ml->data+4*pnodecount;
        inst->u0 = ml->data+5*pnodecount;
        inst->e = ml->data+6*pnodecount;
        inst->gmax = ml->data+7*pnodecount;
        inst->i = ml->data+8*pnodecount;
        inst->g = ml->data+9*pnodecount;
        inst->A = ml->data+10*pnodecount;
        inst->B = ml->data+11*pnodecount;
        inst->factor = ml->data+12*pnodecount;
        inst->DA = ml->data+13*pnodecount;
        inst->DB = ml->data+14*pnodecount;
        inst->v_unused = ml->data+15*pnodecount;
        inst->g_unused = ml->data+16*pnodecount;
        inst->tsave = ml->data+17*pnodecount;
        inst->node_area = nt->_data;
        inst->point_process = ml->pdata;
        inst->rng = nt->_vdata;
        copy_instance_to_device(nt, ml, inst);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, ProbUDFsyn_Instance const* inst) {
        if (!nt->compute_gpu) {
            return;
        }
        auto tmp = *inst;
        auto* d_inst = cnrn_target_is_present(inst);
        if (!d_inst) {
            d_inst = cnrn_target_copyin(inst);
        }
        tmp.global = cnrn_target_deviceptr(tmp.global);
        tmp.tau_r = cnrn_target_deviceptr(tmp.tau_r);
        tmp.tau_d = cnrn_target_deviceptr(tmp.tau_d);
        tmp.Use = cnrn_target_deviceptr(tmp.Use);
        tmp.Dep = cnrn_target_deviceptr(tmp.Dep);
        tmp.Fac = cnrn_target_deviceptr(tmp.Fac);
        tmp.u0 = cnrn_target_deviceptr(tmp.u0);
        tmp.e = cnrn_target_deviceptr(tmp.e);
        tmp.gmax = cnrn_target_deviceptr(tmp.gmax);
        tmp.i = cnrn_target_deviceptr(tmp.i);
        tmp.g = cnrn_target_deviceptr(tmp.g);
        tmp.A = cnrn_target_deviceptr(tmp.A);
        tmp.B = cnrn_target_deviceptr(tmp.B);
        tmp.factor = cnrn_target_deviceptr(tmp.factor);
        tmp.DA = cnrn_target_deviceptr(tmp.DA);
        tmp.DB = cnrn_target_deviceptr(tmp.DB);
        tmp.v_unused = cnrn_target_deviceptr(tmp.v_unused);
        tmp.g_unused = cnrn_target_deviceptr(tmp.g_unused);
        tmp.tsave = cnrn_target_deviceptr(tmp.tsave);
        tmp.node_area = cnrn_target_deviceptr(tmp.node_area);
        tmp.point_process = cnrn_target_deviceptr(tmp.point_process);
        tmp.rng = cnrn_target_deviceptr(tmp.rng);
        cnrn_target_memcpy_to_device(d_inst, &tmp);
        auto* d_ml = cnrn_target_deviceptr(ml);
        void* d_inst_void = d_inst;
        cnrn_target_memcpy_to_device(&(d_ml->instance), &d_inst_void);
    }

    static inline void delete_instance_from_device(ProbUDFsyn_Instance* inst) {
        if (cnrn_target_is_present(inst)) {
            cnrn_target_delete(inst);
        }
    }



    static void nrn_alloc_ProbUDFsyn(double* data, Datum* indexes, int type) {
        // do nothing
    }


    void nrn_constructor_ProbUDFsyn(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<ProbUDFsyn_Instance*>(ml->instance);

        #endif
    }


    void nrn_destructor_ProbUDFsyn(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<ProbUDFsyn_Instance*>(ml->instance);

        #endif
    }


    static inline void net_receive_kernel_ProbUDFsyn(double t, Point_process* pnt, ProbUDFsyn_Instance* inst, NrnThread* nt, Memb_list* ml, int weight_index, double flag) {
        int tid = pnt->_tid;
        int id = pnt->_i_instance;
        double v = 0;
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        double* data = ml->data;
        double* weights = nt->weights;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;

        double* weight = weights + weight_index + 0;
        double* Pv = weights + weight_index + 1;
        double* u = weights + weight_index + 2;
        double* tsyn = weights + weight_index + 3;
        double* initialized = weights + weight_index + 4;
        inst->tsave[id] = t;
        {
            double Pv_tmp, Pr;
            if ((*initialized) == 0.0) {
                (*Pv) = 1.0;
                (*u) = inst->u0[id];
                (*tsyn) = 0.0;
                (*initialized) = 1.0;
            }
            if (inst->Fac[id] > 0.0) {
                (*u) = (*u) * exp( -(t - (*tsyn)) / inst->Fac[id]);
                (*u) = (*u) + inst->Use[id] * (1.0 - (*u));
            } else {
                (*u) = inst->Use[id];
            }
            Pv_tmp = 1.0 - (1.0 - (*Pv)) * exp( -(t - (*tsyn)) / inst->Dep[id]);
            Pr = (*u) * Pv_tmp;
            Pv_tmp = Pv_tmp - (*u) * Pv_tmp;
            if (nrnran123_negexp((nrnran123_State*) inst->rng[indexes[2*pnodecount + id]]) < Pr) {
                (*tsyn) = t;
                (*Pv) = Pv_tmp;
                inst->A[id] = inst->A[id] + (*weight) * inst->factor[id];
                inst->B[id] = inst->B[id] + (*weight) * inst->factor[id];
            }
        }
    }


    static void net_receive_ProbUDFsyn(Point_process* pnt, int weight_index, double flag) {
        NrnThread* nt = nrn_threads + pnt->_tid;
        Memb_list* ml = get_memb_list(nt);
        NetReceiveBuffer_t* nrb = ml->_net_receive_buffer;
        if (nrb->_cnt >= nrb->_size) {
            realloc_net_receive_buffer(nt, ml);
        }
        int id = nrb->_cnt;
        nrb->_pnt_index[id] = pnt-nt->pntprocs;
        nrb->_weight_index[id] = weight_index;
        nrb->_nrb_t[id] = nt->_t;
        nrb->_nrb_flag[id] = flag;
        nrb->_cnt++;
    }


    void net_buf_receive_ProbUDFsyn(NrnThread* nt) {
        Memb_list* ml = get_memb_list(nt);
        if (!ml) {
            return;
        }

        nrn_pragma_acc(data present(nt, ml) if(nt->compute_gpu))
        {
            NetReceiveBuffer_t* nrb = ml->_net_receive_buffer;
            auto* const inst = static_cast<ProbUDFsyn_Instance*>(ml->instance);
            int count = nrb->_displ_cnt;
            nrn_pragma_acc(parallel loop present(inst, nrb) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int i = 0; i < count; i++) {
                int start = nrb->_displ[i];
                int end = nrb->_displ[i+1];
                for (int j = start; j < end; j++) {
                    int index = nrb->_nrb_index[j];
                    int offset = nrb->_pnt_index[index];
                    double t = nrb->_nrb_t[index];
                    int weight_index = nrb->_weight_index[index];
                    double flag = nrb->_nrb_flag[index];
                    Point_process* point_process = nt->pntprocs + offset;
                    net_receive_kernel_ProbUDFsyn(t, point_process, inst, nt, ml, weight_index, flag);
                }
            }
            if(nt->compute_gpu) {
                nrn_pragma_acc(wait(nt->stream_id))
            }
            nrb->_displ_cnt = 0;
            nrb->_cnt = 0;
        }
    }


    /** initialize channel */
    void nrn_init_ProbUDFsyn(NrnThread* nt, Memb_list* ml, int type) {
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
            auto* const inst = static_cast<ProbUDFsyn_Instance*>(ml->instance);

            if (nt->compute_gpu) {
                nrn_pragma_acc(update device (ProbUDFsyn_global))
                nrn_pragma_omp(target update to(ProbUDFsyn_global))
            }
            if (_nrn_skip_initmodel == 0) {
                nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread) async(nt->stream_id) if(nt->compute_gpu))
                nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
                for (int id = 0; id < nodecount; id++) {
                    inst->tsave[id] = -1e20;
                    int node_id = node_index[id];
                    double v = voltage[node_id];
                    #if NRN_PRCELLSTATE
                    inst->v_unused[id] = v;
                    #endif
                    inst->A[id] = inst->global->A0;
                    inst->B[id] = inst->global->B0;
                    double tp;
                    inst->A[id] = 0.0;
                    inst->B[id] = 0.0;
                    tp = (inst->tau_r[id] * inst->tau_d[id]) / (inst->tau_d[id] - inst->tau_r[id]) * log(inst->tau_d[id] / inst->tau_r[id]);
                    inst->factor[id] = 1.0 / ( -exp( -tp / inst->tau_r[id]) + exp( -tp / inst->tau_d[id]));
                    nrnran123_setseq((nrnran123_State*) inst->rng[indexes[2*pnodecount + id]], 0.0);
                }
            }
        }
    }


    inline double nrn_current_ProbUDFsyn(int id, int pnodecount, ProbUDFsyn_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        double current = 0.0;
        inst->g[id] = inst->gmax[id] * (inst->B[id] - inst->A[id]);
        inst->i[id] = inst->g[id] * (v - inst->e[id]);
        current += inst->i[id];
        return current;
    }


    /** update current */
    void nrn_cur_ProbUDFsyn(NrnThread* nt, Memb_list* ml, int type) {
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
            auto* const inst = static_cast<ProbUDFsyn_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread, vec_rhs, vec_d) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                double g = nrn_current_ProbUDFsyn(id, pnodecount, inst, data, indexes, thread, nt, v+0.001);
                double rhs = nrn_current_ProbUDFsyn(id, pnodecount, inst, data, indexes, thread, nt, v);
                g = (g-rhs)/0.001;
                double mfactor = 1.e2/inst->node_area[indexes[0*pnodecount + id]];
                g = g*mfactor;
                rhs = rhs*mfactor;
                #if NRN_PRCELLSTATE
                inst->g_unused[id] = g;
                #endif
                nrn_pragma_acc(atomic update)
                nrn_pragma_omp(atomic update)
                vec_rhs[node_id] -= rhs;
                nrn_pragma_acc(atomic update)
                nrn_pragma_omp(atomic update)
                vec_d[node_id] += g;
            }
        }
    }


    /** update state */
    void nrn_state_ProbUDFsyn(NrnThread* nt, Memb_list* ml, int type) {
        nrn_pragma_acc(data present(nt, ml) if(nt->compute_gpu))
        {
            int nodecount = ml->nodecount;
            int pnodecount = ml->_nodecount_padded;
            const int* node_index = ml->nodeindices;
            double* data = ml->data;
            const double* voltage = nt->_actual_v;
            Datum* indexes = ml->pdata;
            ThreadDatum* thread = ml->_thread;
            auto* const inst = static_cast<ProbUDFsyn_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                inst->A[id] = inst->A[id] + (1.0 - exp(nt->_dt * (( -1.0) / inst->tau_r[id]))) * ( -(0.0) / (( -1.0) / inst->tau_r[id]) - inst->A[id]);
                inst->B[id] = inst->B[id] + (1.0 - exp(nt->_dt * (( -1.0) / inst->tau_d[id]))) * ( -(0.0) / (( -1.0) / inst->tau_d[id]) - inst->B[id]);
            }
        }
    }


    /** register channel with the simulator */
    void _ProbUDFsyn_reg() {

        int mech_type = nrn_get_mechtype("ProbUDFsyn");
        ProbUDFsyn_global.mech_type = mech_type;
        if (mech_type == -1) {
            return;
        }

        _nrn_layout_reg(mech_type, 0);
        point_register_mech(mechanism_info, nrn_alloc_ProbUDFsyn, nrn_cur_ProbUDFsyn, nullptr, nrn_state_ProbUDFsyn, nrn_init_ProbUDFsyn, nrn_private_constructor_ProbUDFsyn, nrn_private_destructor_ProbUDFsyn, first_pointer_var_index(), nullptr, nullptr, 1);

        hoc_register_prop_size(mech_type, float_variables_size(), int_variables_size());
        hoc_register_dparam_semantics(mech_type, 0, "area");
        hoc_register_dparam_semantics(mech_type, 1, "pntproc");
        hoc_register_dparam_semantics(mech_type, 2, "random");
        hoc_register_net_receive_buffering(net_buf_receive_ProbUDFsyn, mech_type);
        set_pnt_receive(mech_type, net_receive_ProbUDFsyn, nullptr, num_net_receive_args());
        hoc_register_var(hoc_scalar_double, hoc_vector_double, NULL);
    }
}
