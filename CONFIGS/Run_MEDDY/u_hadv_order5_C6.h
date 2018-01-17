!
!===============================================================
!
! Compute 5th order horizontal advection
!
!===============================================================
!
#  ifdef NS_PERIODIC
          jmin=1
          jmax=LOCALMM+1
#  else
#   ifdef MPI
          if (SOUTH_INTER) then
            jmin=1
          else
            jmin=3
          endif
          if (NORTH_INTER) then
            jmax=Mmmpi+1
          else
            jmax=Mmmpi-1
          endif
#   else
          jmin=3
          jmax=Mm-1
#   endif
#  endif
#  ifdef EW_PERIODIC
          imin=0
          imax=LOCALLM+1
#  else
#   ifdef MPI
          if (WEST_INTER) then
            imin=0
          else
            imin=3
          endif
          if (EAST_INTER) then
            imax=Lmmpi+1
          else
            imax=Lmmpi-1
          endif
#   else
          imin=3
          imax=Lm-1
#   endif
#  endif
!
!----------------------------------------------------------------------
!  j loop: UFe
!----------------------------------------------------------------------
!

          DO j = Jstr,Jend+1  !j_loop_y_flux_5
                                                  !
            IF ( j.ge.jmin .and. j.le.jmax ) THEN ! use full stencil
                                                  !
              DO i = IstrU,Iend
                vel = flux6(Hvom(i-3,j,k),Hvom(i-2,j,k),Hvom(i-1,j,k),
     &                      Hvom(i,j,k),Hvom(i+1,j,k),Hvom(i+2,j,k),1.)
!               vel = 0.5*(Hvom(i-1,j,k)+Hvom(i,j,k))
                flx5 = vel*flux6(
     &             u(i,j-3,k,nrhs), u(i,j-2,k,nrhs), 
     &             u(i,j-1,k,nrhs), u(i,j  ,k,nrhs),
     &             u(i,j+1,k,nrhs), u(i,j+2,k,nrhs), vel )
#  ifdef MASKING
                vel = flux4(Hvom(i-2,j,k),Hvom(i-1,j,k),
     &                      Hvom(i,j,k),Hvom(i+1,j,k),1.)
                flx3 = vel*flux4(
     &             u(i,j-2,k,nrhs), u(i,j-1,k,nrhs),
     &             u(i,j  ,k,nrhs), u(i,j+1,k,nrhs), vel ) 
                vel = 0.5*(Hvom(i-1,j,k)+Hvom(i,j,k))
                flx2 = vel*FLUX2(
     &             u(i,j-1,k,nrhs), u(i,j  ,k,nrhs), vel, cdif)
#   ifdef UP5_MASKING
                mask0=umask(i,j-1)*umask(i,j)
                mask2=umask(i,j-2)*mask0*umask(i,j+1)
                IF (vel.gt.0) THEN
                  mask1=umask(i,j-2)*mask0
                  mask3=umask(i,j-3)*mask2          
                ELSE
                  mask1=umask(i,j+1)*mask0
                  mask3=umask(i,j+2)*mask2
                ENDIF
                UFeC(i,j)=mask3*flx5+(1-mask3)*mask1*flx3+
     &                             (1-mask3)*(1-mask1)*mask0*flx2
#   else
                mask1=umask(i,j-2)*umask(i,j+1)
                mask2=umask(i,j-3)*umask(i,j+2)
                mask0=mask1*mask2
                UFeC(i,j)=mask0*flx5+(1-mask0)*mask1*flx3+
     &                         (1-mask0)*(1-mask1)*flx2
#   endif /* UP5_MASKING */
#  else
                UFeC(i,j)=flx5
#  endif /* MASKING */
              ENDDO
                                           !
            ELSE IF ( j.eq.jmin-2 ) THEN   ! 2nd order flux next to south
                                           ! boundary
              DO i = IstrU,Iend
                vel = 0.5*(Hvom(i-1,j,k)+ Hvom(i,j,k))
                UFeC(i,j) = vel*flux2(
     &             u(i,j-1,k,nrhs), u(i,j,k,nrhs), vel, cdif)
              ENDDO
                                                             !
            ELSE IF ( j.eq.jmin-1 .and. jmax.ge.jmin ) THEN  ! 3rd of 4th order flux 2 in
                                                             ! from south boundary
              DO i = IstrU,Iend
!               vel = 0.5*(Hvom(i-1,j,k)+ Hvom(i,j,k))
                vel = flux4(Hvom(i-2,j,k),Hvom(i-1,j,k),
     &                      Hvom(i,j,k),Hvom(i+1,j,k),1.)
                flx3 = vel*flux4(
     &             u(i,j-2,k,nrhs), u(i,j-1,k,nrhs),
     &             u(i,j  ,k,nrhs), u(i,j+1,k,nrhs), vel )
#  ifdef MASKING
                vel = 0.5*(Hvom(i-1,j,k)+ Hvom(i,j,k))
                flx2 = vel*FLUX2(
     &             u(i,j-1,k,nrhs), u(i,j  ,k,nrhs), vel, cdif)
                mask1=umask(i,j-2)*umask(i,j+1)
                UFeC(i,j)=mask1*flx3+(1-mask1)*flx2
#  else
                UFeC(i,j)=flx3
#  endif
              ENDDO

            ELSE IF ( j.eq.jmax+2 ) THEN  ! 2nd order flux next to north
                                          ! boundary
              DO i = IstrU,Iend
                vel = 0.5*(Hvom(i-1,j,k)+ Hvom(i,j,k))
                UFeC(i,j) = vel*flux2(
     &             u(i,j-1,k,nrhs), u(i,j,k,nrhs), vel, cdif)
              ENDDO
                                          !
            ELSE IF ( j.eq.jmax+1 ) THEN  ! 3rd or 4th order flux 2 in from
                                          ! north boundary
              DO i = IstrU,Iend
!               vel = 0.5*(Hvom(i-1,j,k)+ Hvom(i,j,k))
                vel = flux4(Hvom(i-2,j,k),Hvom(i-1,j,k),
     &                      Hvom(i,j,k),Hvom(i+1,j,k),1.)
                flx3 = vel*flux4(
     &             u(i,j-2,k,nrhs), u(i,j-1,k,nrhs),
     &             u(i,j  ,k,nrhs), u(i,j+1,k,nrhs), vel )
#  ifdef MASKING
                vel = 0.5*(Hvom(i-1,j,k)+ Hvom(i,j,k))
                flx2 = vel*FLUX2(
     &             u(i,j-1,k,nrhs), u(i,j  ,k,nrhs), vel, cdif)
                mask1=umask(i,j-2)*umask(i,j+1)
                UFeC(i,j)=mask1*flx3+(1-mask1)*flx2
#  else
                UFeC(i,j)=flx3
#  endif
              ENDDO
            ENDIF
          ENDDO ! j_loop_y_flux_5
!
!----------------------------------------------------------------------
!  i loop: UFx
!----------------------------------------------------------------------
!
          DO i = IstrU-1,Iend  !i_loop_x_flux_5
                                                  !
            IF ( i.ge.imin .and. i.le.imax ) THEN ! use full stencil
                                                  !
              DO j = Jstr,Jend
                vel = flux6(Huon(i-2,j,k),Huon(i-1,j,k),Huon(i,j,k),
     &                      Huon(i+1,j,k),Huon(i+2,j,k),Huon(i+3,j,k),1.)        
!               vel = 0.5*(Huon(i,j,k)+Huon(i+1,j,k))
                flx5 = vel*flux6(
     &             u(i-2,j,k,nrhs), u(i-1,j,k,nrhs),
     &             u(i  ,j,k,nrhs), u(i+1,j,k,nrhs),
     &             u(i+2,j,k,nrhs), u(i+3,j,k,nrhs), vel )
#  ifdef MASKING
                vel = flux4(Huon(i-1,j,k),Huon(i,j,k),
     &                      Huon(i+1,j,k),Huon(i+2,j,k),1.)    
                flx3 = vel*flux4(
     &             u(i-1,j,k,nrhs), u(i  ,j,k,nrhs),
     &             u(i+1,j,k,nrhs), u(i+2,j,k,nrhs), vel )       
                vel = 0.5*(Huon(i,j,k)+Huon(i+1,j,k))
                flx2 = vel*FLUX2(
     &             u(i  ,j,k,nrhs), u(i+1,j,k,nrhs), vel, cdif)
#   ifdef UP5_MASKING
                mask0=umask(i,j)*umask(i+1,j)
                mask2=umask(i-1,j)*mask0*umask(i+2,j)
                IF (vel.gt.0) THEN
                  mask1=umask(i-1,j)*mask0
                  mask3=umask(i-2,j)*mask2          
                ELSE
                  mask1=umask(i+2,j)*mask0
                  mask3=umask(i+3,j)*mask2
                ENDIF
                UFxC(i,j)=mask3*flx5+(1-mask3)*mask1*flx3+
     &                             (1-mask3)*(1-mask1)*mask0*flx2
#   else
                mask1=umask(i-1,j)*umask(i+2,j)
                mask2=umask(i-2,j)*umask(i+3,j)
                mask0=mask1*mask2
                UFxC(i,j)=mask0*flx5+(1-mask0)*mask1*flx3+
     &                         (1-mask0)*(1-mask1)*flx2
#   endif /* UP5_MASKING */
#  else
                UFxC(i,j)=flx5
#  endif /* MASKING */
              ENDDO
                                           !
            ELSE IF ( i.eq.imin-2 ) THEN   ! 2nd order flux next to south
                                           ! boundary
              DO j = Jstr,Jend
                vel = 0.5*(Huon(i,j,k)+Huon(i+1,j,k))
                UFxC(i,j) = vel*flux2(
     &             u(i,j,k,nrhs), u(i+1,j,k,nrhs), vel, cdif)
              ENDDO
                                                             !
            ELSE IF ( i.eq.imin-1 .and. imax.ge.imin ) THEN  ! 3rd of 4th order flux 2 in
                                                             ! from south boundary
              DO j = Jstr,Jend
!               vel = 0.5*(Huon(i,j,k)+Huon(i+1,j,k))
                vel = flux4(Huon(i-1,j,k),Huon(i,j,k),
     &                      Huon(i+1,j,k),Huon(i+2,j,k),1.)    
                flx3 = vel*flux4(
     &             u(i-1,j,k,nrhs), u(i  ,j,k,nrhs),
     &             u(i+1,j,k,nrhs), u(i+2,j,k,nrhs), vel )
#  ifdef MASKING
                vel = 0.5*(Huon(i,j,k)+Huon(i+1,j,k))
                flx2 = vel*FLUX2(
     &             u(i  ,j,k,nrhs), u(i+1,j,k,nrhs), vel, cdif)
                mask1=umask(i-1,j)*umask(i+2,j)
                UFxC(i,j)=mask1*flx3+(1-mask1)*flx2
#  else
                UFxC(i,j)=flx3
#  endif
              ENDDO
                                          !
            ELSE IF ( i.eq.imax+2 ) THEN  ! 2nd order flux next to north
                                          ! boundary
              DO j = Jstr,Jend
                vel = 0.5*(Huon(i,j,k)+Huon(i+1,j,k))
                UFxC(i,j) = vel*flux2(
     &             u(i,j,k,nrhs), u(i+1,j,k,nrhs), vel, cdif)
              ENDDO
                                          !
            ELSE IF ( i.eq.imax+1 ) THEN  ! 3rd or 4th order flux 2 in from
                                          ! north boundary
              DO j = Jstr,Jend
!               vel = 0.5*(Huon(i,j,k)+Huon(i+1,j,k))
                vel = flux4(Huon(i-1,j,k),Huon(i,j,k),
     &                      Huon(i+1,j,k),Huon(i+2,j,k),1.)  
                flx3 = vel*flux4(
     &             u(i-1,j,k,nrhs), u(i  ,j,k,nrhs),
     &             u(i+1,j,k,nrhs), u(i+2,j,k,nrhs),  vel )
#  ifdef MASKING
                vel = 0.5*(Huon(i,j,k)+Huon(i+1,j,k))
                flx2 = vel*flux2(
     &             u(i,j,k,nrhs), u(i+1,j,k,nrhs), vel, cdif)
                mask1=umask(i-1,j)*umask(i+2,j)
                UFxC(i,j)=mask1*flx3+(1-mask1)*flx2
#  else
                UFxC(i,j)=flx3
#  endif
              ENDDO
            ENDIF
          ENDDO ! i_loop_x_flux_5

