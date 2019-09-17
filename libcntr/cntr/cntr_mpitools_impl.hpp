#ifndef CNTR_MPITOOLS_IMPL_H
#define CNTR_MPITOOLS_IMPL_H

#include "cntr_mpitools_decl.hpp"
#include "cntr_herm_matrix_decl.hpp"
#include "cntr_herm_matrix_timestep_decl.hpp"

namespace cntr{

/** \brief <b> MPI reduce for the `herm_matrix_timestep` to a `herm_matrix_timestep` </b>
*
* <!-- ====== DOCUMENTATION ====== -->
*
*  \par Purpose
* <!-- ========= -->
*
* > MPI reduce for the `herm_matrix_timestep` to the `root`
* > Works for scalar or square-matrix contour objects.
*
* <!-- ARGUMENTS
*      ========= -->
*
* @param tstp
* > time step
* @param root
* > Index of root
* @param Gred
* > The reduced `herm_matrix_timestep` on rank `root`.
* @param G
* > The `herm_matrix_timestep` on the individual ranks.
*/
template <typename T> void Reduce_timestep(int tstp, int root, herm_matrix_timestep<T> &Gred, 
	herm_matrix_timestep<T> G){
	assert(tstp == G.tstp());
	int taskid;
	taskid = MPI::COMM_WORLD.Get_rank();
	if (taskid == root) {
		assert(tstp == Gred.tstp());
		assert(G.ntau() == Gred.ntau());
		assert(G.size1() == Gred.size1());
		assert(G.size2() == Gred.size2());
	}

	int len = 2 * (2 * (tstp + 1) + G.ntau() + 1) * G.size1() * G.size2();

	if (sizeof(T) == sizeof(double)) {
      if (taskid == root) {
         MPI::COMM_WORLD.Reduce(MPI::IN_PLACE, (double *) Gred.data_, len,
            MPI::DOUBLE, MPI::SUM, root);
      } else {
         MPI::COMM_WORLD.Reduce((double *)G.data_,
            (double *)Gred.data_, len, MPI::DOUBLE,
            MPI::SUM, root);
      }
   } else {
      std::cerr << "herm_matrix_timestep<T>::MPI_Reduce only for double "
      << std::endl;
      exit(0);
   }

}

/** \brief <b> MPI reduce for the `herm_matrix_timestep` to a `herm_matrix` </b>
*
* <!-- ====== DOCUMENTATION ====== -->
*
*  \par Purpose
* <!-- ========= -->
*
* > MPI reduce for the `herm_matrix_timestep` to a `herm_matrix` on rank `root`.
* > Works for scalar or square-matrix contour objects.
*
* <!-- ARGUMENTS
*      ========= -->
*
* @param tstp
* > time step
* @param root
* > Index of root
* @param Gred
* > The reduced `herm_matrix` on rank `root`.
* @param G
* > The `herm_matrix_timestep` on the individual ranks.
*/
template <typename T> void Reduce_timestep(int tstp, int root, herm_matrix<T> &Gred, 
	herm_matrix_timestep<T> G){
	assert(tstp == G.tstp());
	int taskid = MPI::COMM_WORLD.Get_rank();
	if (taskid == root) {
		assert(tstp <= Gred.nt());
		assert(G.ntau() == Gred.ntau());
		assert(G.size1() == Gred.size1());
		assert(G.size2() == Gred.size2());
	}

    herm_matrix_timestep<T> Gtemp;
	if (taskid == root){
	    Gtemp.resize(tstp, G.ntau(), G.size1());
	}

	Reduce_timestep(tstp, root, Gtemp, G);

	if (taskid == root){
		Gred.set_timestep(tstp, Gtemp);
	}
}

/** \brief <b> MPI reduce for the `herm_matrix` to a `herm_matrix_timestep` </b>
*
* <!-- ====== DOCUMENTATION ====== -->
*
*  \par Purpose
* <!-- ========= -->
*
* > MPI reduce for the `herm_matrix` to a `herm_matrix_timestep` on rank `root`.
* > Works for scalar or square-matrix contour objects.
*
* <!-- ARGUMENTS
*      ========= -->
*
* @param tstp
* > time step
* @param root
* > Index of root
* @param Gred
* > The reduced `herm_matrix_timestep` on rank `root`.
* @param G
* > The `herm_matrix` on the individual ranks.
*/
template <typename T> void Reduce_timestep(int tstp, int root, herm_matrix_timestep<T> &Gred, 
	herm_matrix<T> G){
	assert(tstp <= G.nt());
	int taskid = MPI::COMM_WORLD.Get_rank();
	if (taskid == root) {
		assert(tstp == Gred.tstp());
		assert(G.ntau() == Gred.ntau());
		assert(G.size1() == Gred.size1());
		assert(G.size2() == Gred.size2());
	}

	herm_matrix_timestep<T> Gtemp;
	Gtemp.resize(tstp, G.ntau(), G.size1());
	G.get_timestep(tstp, Gtemp);

	Reduce_timestep(tstp, root, Gred, Gtemp);

}


/** \brief <b> MPI reduce for the `herm_matrix` to a `herm_matrix` </b>
*
* <!-- ====== DOCUMENTATION ====== -->
*
*  \par Purpose
* <!-- ========= -->
*
* > MPI reduce for the `herm_matrix` to a `herm_matrix` on rank `root`.
* > Works for scalar or square-matrix contour objects.
*
* <!-- ARGUMENTS
*      ========= -->
*
* @param tstp
* > time step
* @param root
* > Index of root
* @param Gred
* > The reduced `herm_matrix` on rank `root`.
* @param G
* > The `herm_matrix` on the individual ranks.
*/
template <typename T> void Reduce_timestep(int tstp, int root, herm_matrix<T> &Gred, 
	herm_matrix<T> G){
	assert(tstp <= G.nt());
	int taskid = MPI::COMM_WORLD.Get_rank();
	if (taskid == root) {
		assert(tstp <= Gred.nt());
		assert(G.ntau() == Gred.ntau());
		assert(G.size1() == Gred.size1());
		assert(G.size2() == Gred.size2());
	}

	herm_matrix_timestep<T> Gred_tmp;
	if (taskid == root){
		Gred_tmp.resize(tstp, G.ntau(), G.size1());
	}

	herm_matrix_timestep<T> G_tmp;
	G_tmp.resize(tstp, G.ntau(), G.size1());

	Reduce_timestep(tstp, root, Gred_tmp, G_tmp);

	if (taskid == root){
		Gred.set_timestep(tstp, Gred_tmp);
	}
}



} // namespace cntr


#endif // CNTR_MPITOOLS_IMPL_H