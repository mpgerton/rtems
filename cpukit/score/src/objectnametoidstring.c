/*
 *  Object Handler - Object ID to Name (String)
 *
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include <rtems/system.h>
#include <rtems/score/address.h>
#include <rtems/score/chain.h>
#include <rtems/score/object.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/objectmp.h>
#endif
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/isr.h>

/*PAGE
 *
 *  _Objects_Name_to_id_string
 *
 *  These kernel routines search the object table(s) for the given
 *  object name and returns the associated object id.
 *
 *  Input parameters:
 *    information - object information
 *    name        - user defined object name
 *    node        - node indentifier (0 indicates any node)
 *    id          - address of return ID
 *
 *  Output parameters:
 *    id                                   - object id
 *    OBJECTS_NAME_OR_ID_LOOKUP_SUCCESSFUL - if successful
 *    error code                           - if unsuccessful
 */

Objects_Name_or_id_lookup_errors _Objects_Name_to_id_string(
  Objects_Information *information,
  const char          *name,
  uint32_t             node,
  Objects_Id          *id
)
{
  boolean                    search_local_node;
  Objects_Control           *the_object;
  uint32_t                   index;
  uint32_t                   name_length;
#if defined(RTEMS_MULTIPROCESSING)
  Objects_Name               name_for_mp;
#endif

  /* ASSERT: information->is_string == TRUE */

  if ( !id )
    return OBJECTS_INVALID_ADDRESS;

  if ( !name )
    return OBJECTS_INVALID_NAME;

  search_local_node = FALSE;

  if ( information->maximum != 0 &&
      (node == OBJECTS_SEARCH_ALL_NODES ||
       node == OBJECTS_SEARCH_LOCAL_NODE ||
       _Objects_Is_local_node( node )
      ))
   search_local_node = TRUE;

  if ( search_local_node ) {
    name_length = information->name_length;

    for ( index = 1; index <= information->maximum; index++ ) {
      the_object = information->local_table[ index ];
      if ( !the_object )
        continue;

      if ( !the_object->name.name_p )
        continue;

      if (!strncmp( name, the_object->name.name_p, information->name_length)) {
        *id = the_object->id;
        return OBJECTS_NAME_OR_ID_LOOKUP_SUCCESSFUL;
      }
    }
  }

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Objects_Is_local_node( node ) || node == OBJECTS_SEARCH_LOCAL_NODE )
    return OBJECTS_INVALID_NAME;

  name_for_mp.name_p = name;
  return _Objects_MP_Global_name_search( information, name_for_mp, node, id );
#else
  return OBJECTS_INVALID_NAME;
#endif
}
