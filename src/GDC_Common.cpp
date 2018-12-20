#include "GDC_Common.h"

static const pf_plane_table_t*  p_plane_table;
static uint16_t                 plane_table_count;

void PF_GDC_SetPlaneTable(const pf_plane_table_t *plane_table, uint16_t count)
{
  p_plane_table = plane_table;
  plane_table_count = count;
}

const pf_plane_table_t *PF_GDC_GetPlaneTable(void)
{
  return p_plane_table;
}

uint16_t PF_GDC_GetPlaneTableCount(void)
{
  return plane_table_count;
}