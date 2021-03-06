/**
 * @file
 *
 * @brief Function Unlocks a Spin Lock Object
 * @ingroup POSIXAPI
 */

/*
 *  POSIX Spinlock Manager -- Wait at a Spinlock
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/spinlockimpl.h>
#include <rtems/posix/posixapi.h>

int pthread_spin_unlock( pthread_spinlock_t *spinlock )
{
  POSIX_Spinlock_Control *the_spinlock;
  ISR_lock_Context        lock_context;
  Status_Control          status;

  the_spinlock = _POSIX_Spinlock_Get( spinlock, &lock_context );
  if ( the_spinlock == NULL ) {
    return EINVAL;
  }

  status = _CORE_spinlock_Surrender( &the_spinlock->Spinlock, &lock_context );
  return _POSIX_Get_error( status );
}
