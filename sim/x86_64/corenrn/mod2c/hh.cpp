/*********************************************************
Model Name      : hh
Filename        : hh.mod
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
        "hh",
        "gnabar_hh",
        "gkbar_hh",
        "gl_hh",
        "el_hh",
        0,
        "gna_hh",
        "gk_hh",
        "il_hh",
        "minf_hh",
        "hinf_hh",
        "ninf_hh",
        "mtau_hh",
        "htau_hh",
        "ntau_hh",
        0,
        "m_hh",
        "h_hh",
        "n_hh",
        0,
        0
    };


    /** all global variables */
    struct hh_Store {
        int na_type{};
        int k_type{};
        double m0{};
        double h0{};
        double n0{};
        int reset{};
        int mech_type{};
        int slist1[3]{13, 14, 15};
        int dlist1[3]{16, 17, 18};
        double usetable{1};
        double tmin_rates{};
        double mfac_rates{};
        double t_minf[201]{};
        double t_hinf[201]{};
        double t_ninf[201]{};
        double t_mtau[201]{};
        double t_htau[201]{};
        double t_ntau[201]{};
    };
    static_assert(std::is_trivially_copy_constructible_v<hh_Store>);
    static_assert(std::is_trivially_move_constructible_v<hh_Store>);
    static_assert(std::is_trivially_copy_assignable_v<hh_Store>);
    static_assert(std::is_trivially_move_assignable_v<hh_Store>);
    static_assert(std::is_trivially_destructible_v<hh_Store>);
    static hh_Store hh_global;


    /** all mechanism instance variables and global variables */
    struct hh_Instance  {
        double* celsius{&coreneuron::celsius};
        const double* gnabar{};
        const double* gkbar{};
        const double* gl{};
        const double* el{};
        double* gna{};
        double* gk{};
        double* il{};
        double* minf{};
        double* hinf{};
        double* ninf{};
        double* mtau{};
        double* htau{};
        double* ntau{};
        double* m{};
        double* h{};
        double* n{};
        double* Dm{};
        double* Dh{};
        double* Dn{};
        double* ena{};
        double* ek{};
        double* ina{};
        double* ik{};
        double* v_unused{};
        double* g_unused{};
        const double* ion_ena{};
        double* ion_ina{};
        double* ion_dinadv{};
        const double* ion_ek{};
        double* ion_ik{};
        double* ion_dikdv{};
        hh_Store* global{&hh_global};
    };


    /** connect global (scalar) variables to hoc -- */
    static DoubScal hoc_scalar_double[] = {
        {"usetable_hh", &hh_global.usetable},
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
        return 25;
    }


    static inline int int_variables_size() {
        return 6;
    }


    static inline int get_mech_type() {
        return hh_global.mech_type;
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
    static void nrn_private_constructor_hh(NrnThread* nt, Memb_list* ml, int type) {
        assert(!ml->instance);
        assert(!ml->global_variables);
        assert(ml->global_variables_size == 0);
        auto* const inst = new hh_Instance{};
        assert(inst->global == &hh_global);
        ml->instance = inst;
        ml->global_variables = inst->global;
        ml->global_variables_size = sizeof(hh_Store);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, hh_Instance const* inst);
    static inline void delete_instance_from_device(hh_Instance* inst);
    // Deallocate the instance structure
    static void nrn_private_destructor_hh(NrnThread* nt, Memb_list* ml, int type) {
        auto* const inst = static_cast<hh_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &hh_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(hh_Store));
        delete_instance_from_device(inst);
        delete inst;
        ml->instance = nullptr;
        ml->global_variables = nullptr;
        ml->global_variables_size = 0;
    }

    /** initialize mechanism instance variables */
    static inline void setup_instance(NrnThread* nt, Memb_list* ml) {
        auto* const inst = static_cast<hh_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &hh_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(hh_Store));
        int pnodecount = ml->_nodecount_padded;
        Datum* indexes = ml->pdata;
        inst->gnabar = ml->data+0*pnodecount;
        inst->gkbar = ml->data+1*pnodecount;
        inst->gl = ml->data+2*pnodecount;
        inst->el = ml->data+3*pnodecount;
        inst->gna = ml->data+4*pnodecount;
        inst->gk = ml->data+5*pnodecount;
        inst->il = ml->data+6*pnodecount;
        inst->minf = ml->data+7*pnodecount;
        inst->hinf = ml->data+8*pnodecount;
        inst->ninf = ml->data+9*pnodecount;
        inst->mtau = ml->data+10*pnodecount;
        inst->htau = ml->data+11*pnodecount;
        inst->ntau = ml->data+12*pnodecount;
        inst->m = ml->data+13*pnodecount;
        inst->h = ml->data+14*pnodecount;
        inst->n = ml->data+15*pnodecount;
        inst->Dm = ml->data+16*pnodecount;
        inst->Dh = ml->data+17*pnodecount;
        inst->Dn = ml->data+18*pnodecount;
        inst->ena = ml->data+19*pnodecount;
        inst->ek = ml->data+20*pnodecount;
        inst->ina = ml->data+21*pnodecount;
        inst->ik = ml->data+22*pnodecount;
        inst->v_unused = ml->data+23*pnodecount;
        inst->g_unused = ml->data+24*pnodecount;
        inst->ion_ena = nt->_data;
        inst->ion_ina = nt->_data;
        inst->ion_dinadv = nt->_data;
        inst->ion_ek = nt->_data;
        inst->ion_ik = nt->_data;
        inst->ion_dikdv = nt->_data;
        copy_instance_to_device(nt, ml, inst);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, hh_Instance const* inst) {
        if (!nt->compute_gpu) {
            return;
        }
        auto tmp = *inst;
        auto* d_inst = cnrn_target_is_present(inst);
        if (!d_inst) {
            d_inst = cnrn_target_copyin(inst);
        }
        tmp.global = cnrn_target_deviceptr(tmp.global);
        tmp.celsius = cnrn_target_deviceptr(tmp.celsius);
        tmp.gnabar = cnrn_target_deviceptr(tmp.gnabar);
        tmp.gkbar = cnrn_target_deviceptr(tmp.gkbar);
        tmp.gl = cnrn_target_deviceptr(tmp.gl);
        tmp.el = cnrn_target_deviceptr(tmp.el);
        tmp.gna = cnrn_target_deviceptr(tmp.gna);
        tmp.gk = cnrn_target_deviceptr(tmp.gk);
        tmp.il = cnrn_target_deviceptr(tmp.il);
        tmp.minf = cnrn_target_deviceptr(tmp.minf);
        tmp.hinf = cnrn_target_deviceptr(tmp.hinf);
        tmp.ninf = cnrn_target_deviceptr(tmp.ninf);
        tmp.mtau = cnrn_target_deviceptr(tmp.mtau);
        tmp.htau = cnrn_target_deviceptr(tmp.htau);
        tmp.ntau = cnrn_target_deviceptr(tmp.ntau);
        tmp.m = cnrn_target_deviceptr(tmp.m);
        tmp.h = cnrn_target_deviceptr(tmp.h);
        tmp.n = cnrn_target_deviceptr(tmp.n);
        tmp.Dm = cnrn_target_deviceptr(tmp.Dm);
        tmp.Dh = cnrn_target_deviceptr(tmp.Dh);
        tmp.Dn = cnrn_target_deviceptr(tmp.Dn);
        tmp.ena = cnrn_target_deviceptr(tmp.ena);
        tmp.ek = cnrn_target_deviceptr(tmp.ek);
        tmp.ina = cnrn_target_deviceptr(tmp.ina);
        tmp.ik = cnrn_target_deviceptr(tmp.ik);
        tmp.v_unused = cnrn_target_deviceptr(tmp.v_unused);
        tmp.g_unused = cnrn_target_deviceptr(tmp.g_unused);
        tmp.ion_ena = cnrn_target_deviceptr(tmp.ion_ena);
        tmp.ion_ina = cnrn_target_deviceptr(tmp.ion_ina);
        tmp.ion_dinadv = cnrn_target_deviceptr(tmp.ion_dinadv);
        tmp.ion_ek = cnrn_target_deviceptr(tmp.ion_ek);
        tmp.ion_ik = cnrn_target_deviceptr(tmp.ion_ik);
        tmp.ion_dikdv = cnrn_target_deviceptr(tmp.ion_dikdv);
        cnrn_target_memcpy_to_device(d_inst, &tmp);
        auto* d_ml = cnrn_target_deviceptr(ml);
        void* d_inst_void = d_inst;
        cnrn_target_memcpy_to_device(&(d_ml->instance), &d_inst_void);
    }

    static inline void delete_instance_from_device(hh_Instance* inst) {
        if (cnrn_target_is_present(inst)) {
            cnrn_target_delete(inst);
        }
    }



    static void nrn_alloc_hh(double* data, Datum* indexes, int type) {
        // do nothing
    }


    void nrn_constructor_hh(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<hh_Instance*>(ml->instance);

        #endif
    }


    void nrn_destructor_hh(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<hh_Instance*>(ml->instance);

        #endif
    }


    inline static double vtrap_hh(int id, int pnodecount, hh_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v, double _lx, double _ly);
    inline static int rates_hh(int id, int pnodecount, hh_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v, double _lv);


    inline int f_rates_hh(int id, int pnodecount, hh_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v, double _lv) {
        int ret_f_rates = 0;
        double alpha, beta, sum, q10, vtrap_in_0, vtrap_in_1;
        q10 = pow(3.0, ((*(inst->celsius) - 6.3) / 10.0));
        {
            double _lx_in_0, _ly_in_0;
            _lx_in_0 =  -(_lv + 40.0);
            _ly_in_0 = 10.0;
            if (fabs(_lx_in_0 / _ly_in_0) < 1e-6) {
                vtrap_in_0 = _ly_in_0 * (1.0 - _lx_in_0 / _ly_in_0 / 2.0);
            } else {
                vtrap_in_0 = _lx_in_0 / (exp(_lx_in_0 / _ly_in_0) - 1.0);
            }
        }
        alpha = .1 * vtrap_in_0;
        beta = 4.0 * exp( -(_lv + 65.0) / 18.0);
        sum = alpha + beta;
        inst->mtau[id] = 1.0 / (q10 * sum);
        inst->minf[id] = alpha / sum;
        alpha = .07 * exp( -(_lv + 65.0) / 20.0);
        beta = 1.0 / (exp( -(_lv + 35.0) / 10.0) + 1.0);
        sum = alpha + beta;
        inst->htau[id] = 1.0 / (q10 * sum);
        inst->hinf[id] = alpha / sum;
        {
            double _lx_in_1, _ly_in_1;
            _lx_in_1 =  -(_lv + 55.0);
            _ly_in_1 = 10.0;
            if (fabs(_lx_in_1 / _ly_in_1) < 1e-6) {
                vtrap_in_1 = _ly_in_1 * (1.0 - _lx_in_1 / _ly_in_1 / 2.0);
            } else {
                vtrap_in_1 = _lx_in_1 / (exp(_lx_in_1 / _ly_in_1) - 1.0);
            }
        }
        alpha = .01 * vtrap_in_1;
        beta = .125 * exp( -(_lv + 65.0) / 80.0);
        sum = alpha + beta;
        inst->ntau[id] = 1.0 / (q10 * sum);
        inst->ninf[id] = alpha / sum;
        return ret_f_rates;
    }


    void update_table_rates_hh(int id, int pnodecount, hh_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        if (inst->global->usetable == 0) {
            return;
        }
        static bool make_table = true;
        static double save_celsius;
        if (save_celsius != *(inst->celsius)) {
            make_table = true;
        }
        if (make_table) {
            make_table = false;
            inst->global->tmin_rates =  -100.0;
            double tmax = 100.0;
            double dx = (tmax-inst->global->tmin_rates) / 200.;
            inst->global->mfac_rates = 1./dx;
            double x = inst->global->tmin_rates;
            for (std::size_t i = 0; i < 201; x += dx, i++) {
                f_rates_hh(id, pnodecount, inst, data, indexes, thread, nt, v, x);
                inst->global->t_minf[i] = inst->minf[id];
                inst->global->t_mtau[i] = inst->mtau[id];
                inst->global->t_hinf[i] = inst->hinf[id];
                inst->global->t_htau[i] = inst->htau[id];
                inst->global->t_ninf[i] = inst->ninf[id];
                inst->global->t_ntau[i] = inst->ntau[id];
            }
            save_celsius = *(inst->celsius);
        }
    }


    inline static int rates_hh(int id, int pnodecount, hh_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v, double _lv){
        if (inst->global->usetable == 0) {
            f_rates_hh(id, pnodecount, inst, data, indexes, thread, nt, v, _lv);
            return 0;
        }
        double xi = inst->global->mfac_rates * (_lv - inst->global->tmin_rates);
        if (isnan(xi)) {
            inst->minf[id] = xi;
            inst->mtau[id] = xi;
            inst->hinf[id] = xi;
            inst->htau[id] = xi;
            inst->ninf[id] = xi;
            inst->ntau[id] = xi;
            return 0;
        }
        if (xi <= 0. || xi >= 200.) {
            int index = (xi <= 0.) ? 0 : 200;
            inst->minf[id] = inst->global->t_minf[index];
            inst->mtau[id] = inst->global->t_mtau[index];
            inst->hinf[id] = inst->global->t_hinf[index];
            inst->htau[id] = inst->global->t_htau[index];
            inst->ninf[id] = inst->global->t_ninf[index];
            inst->ntau[id] = inst->global->t_ntau[index];
            return 0;
        }
        int i = int(xi);
        double theta = xi - double(i);
        inst->minf[id] = inst->global->t_minf[i] + theta*(inst->global->t_minf[i+1]-inst->global->t_minf[i]);
        inst->mtau[id] = inst->global->t_mtau[i] + theta*(inst->global->t_mtau[i+1]-inst->global->t_mtau[i]);
        inst->hinf[id] = inst->global->t_hinf[i] + theta*(inst->global->t_hinf[i+1]-inst->global->t_hinf[i]);
        inst->htau[id] = inst->global->t_htau[i] + theta*(inst->global->t_htau[i+1]-inst->global->t_htau[i]);
        inst->ninf[id] = inst->global->t_ninf[i] + theta*(inst->global->t_ninf[i+1]-inst->global->t_ninf[i]);
        inst->ntau[id] = inst->global->t_ntau[i] + theta*(inst->global->t_ntau[i+1]-inst->global->t_ntau[i]);
        return 0;
    }


    inline double vtrap_hh(int id, int pnodecount, hh_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v, double _lx, double _ly) {
        double ret_vtrap = 0.0;
        if (fabs(_lx / _ly) < 1e-6) {
            ret_vtrap = _ly * (1.0 - _lx / _ly / 2.0);
        } else {
            ret_vtrap = _lx / (exp(_lx / _ly) - 1.0);
        }
        return ret_vtrap;
    }


    /** initialize channel */
    void nrn_init_hh(NrnThread* nt, Memb_list* ml, int type) {
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
            auto* const inst = static_cast<hh_Instance*>(ml->instance);

            if (nt->compute_gpu) {
                nrn_pragma_acc(update device (hh_global))
                nrn_pragma_omp(target update to(hh_global))
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
                    inst->ena[id] = inst->ion_ena[indexes[0*pnodecount + id]];
                    inst->ek[id] = inst->ion_ek[indexes[3*pnodecount + id]];
                    inst->m[id] = inst->global->m0;
                    inst->h[id] = inst->global->h0;
                    inst->n[id] = inst->global->n0;
                    rates_hh(id, pnodecount, inst, data, indexes, thread, nt, v, v);
                    inst->m[id] = inst->minf[id];
                    inst->h[id] = inst->hinf[id];
                    inst->n[id] = inst->ninf[id];
                }
            }
        }
    }


    inline double nrn_current_hh(int id, int pnodecount, hh_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        double current = 0.0;
        inst->gna[id] = inst->gnabar[id] * inst->m[id] * inst->m[id] * inst->m[id] * inst->h[id];
        inst->ina[id] = inst->gna[id] * (v - inst->ena[id]);
        inst->gk[id] = inst->gkbar[id] * inst->n[id] * inst->n[id] * inst->n[id] * inst->n[id];
        inst->ik[id] = inst->gk[id] * (v - inst->ek[id]);
        inst->il[id] = inst->gl[id] * (v - inst->el[id]);
        current += inst->il[id];
        current += inst->ina[id];
        current += inst->ik[id];
        return current;
    }


    /** update current */
    void nrn_cur_hh(NrnThread* nt, Memb_list* ml, int type) {
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
            auto* const inst = static_cast<hh_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread, vec_rhs, vec_d) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                inst->ena[id] = inst->ion_ena[indexes[0*pnodecount + id]];
                inst->ek[id] = inst->ion_ek[indexes[3*pnodecount + id]];
                double g = nrn_current_hh(id, pnodecount, inst, data, indexes, thread, nt, v+0.001);
                double dina = inst->ina[id];
                double dik = inst->ik[id];
                double rhs = nrn_current_hh(id, pnodecount, inst, data, indexes, thread, nt, v);
                g = (g-rhs)/0.001;
                inst->ion_dinadv[indexes[2*pnodecount + id]] += (dina-inst->ina[id])/0.001;
                inst->ion_dikdv[indexes[5*pnodecount + id]] += (dik-inst->ik[id])/0.001;
                inst->ion_ina[indexes[1*pnodecount + id]] += inst->ina[id];
                inst->ion_ik[indexes[4*pnodecount + id]] += inst->ik[id];
                #if NRN_PRCELLSTATE
                inst->g_unused[id] = g;
                #endif
                vec_rhs[node_id] -= rhs;
                vec_d[node_id] += g;
            }
        }
    }


    /** update state */
    void nrn_state_hh(NrnThread* nt, Memb_list* ml, int type) {
        nrn_pragma_acc(data present(nt, ml) if(nt->compute_gpu))
        {
            int nodecount = ml->nodecount;
            int pnodecount = ml->_nodecount_padded;
            const int* node_index = ml->nodeindices;
            double* data = ml->data;
            const double* voltage = nt->_actual_v;
            Datum* indexes = ml->pdata;
            ThreadDatum* thread = ml->_thread;
            auto* const inst = static_cast<hh_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                inst->ena[id] = inst->ion_ena[indexes[0*pnodecount + id]];
                inst->ek[id] = inst->ion_ek[indexes[3*pnodecount + id]];
                rates_hh(id, pnodecount, inst, data, indexes, thread, nt, v, v);
                inst->m[id] = inst->m[id] + (1.0 - exp(nt->_dt * (((( -1.0))) / inst->mtau[id]))) * ( -(((inst->minf[id])) / inst->mtau[id]) / (((( -1.0))) / inst->mtau[id]) - inst->m[id]);
                inst->h[id] = inst->h[id] + (1.0 - exp(nt->_dt * (((( -1.0))) / inst->htau[id]))) * ( -(((inst->hinf[id])) / inst->htau[id]) / (((( -1.0))) / inst->htau[id]) - inst->h[id]);
                inst->n[id] = inst->n[id] + (1.0 - exp(nt->_dt * (((( -1.0))) / inst->ntau[id]))) * ( -(((inst->ninf[id])) / inst->ntau[id]) / (((( -1.0))) / inst->ntau[id]) - inst->n[id]);
            }
        }
    }


    static void check_table_thread_hh (int id, int pnodecount, double* data, Datum* indexes, ThreadDatum* thread, NrnThread* nt, Memb_list* ml, int tml_id) {
        setup_instance(nt, ml);
        auto* const inst = static_cast<hh_Instance*>(ml->instance);
        double v = 0;
        update_table_rates_hh(id, pnodecount, inst, data, indexes, thread, nt, v);
    }


    /** register channel with the simulator */
    void _hh_reg() {

        int mech_type = nrn_get_mechtype("hh");
        hh_global.mech_type = mech_type;
        if (mech_type == -1) {
            return;
        }

        _nrn_layout_reg(mech_type, 0);
        register_mech(mechanism_info, nrn_alloc_hh, nrn_cur_hh, nullptr, nrn_state_hh, nrn_init_hh, nrn_private_constructor_hh, nrn_private_destructor_hh, first_pointer_var_index(), 1);
        hh_global.na_type = nrn_get_mechtype("na_ion");
        hh_global.k_type = nrn_get_mechtype("k_ion");

        _nrn_thread_table_reg(mech_type, check_table_thread_hh);
        hoc_register_prop_size(mech_type, float_variables_size(), int_variables_size());
        hoc_register_dparam_semantics(mech_type, 0, "na_ion");
        hoc_register_dparam_semantics(mech_type, 1, "na_ion");
        hoc_register_dparam_semantics(mech_type, 2, "na_ion");
        hoc_register_dparam_semantics(mech_type, 3, "k_ion");
        hoc_register_dparam_semantics(mech_type, 4, "k_ion");
        hoc_register_dparam_semantics(mech_type, 5, "k_ion");
        hoc_register_var(hoc_scalar_double, hoc_vector_double, NULL);
    }
}
