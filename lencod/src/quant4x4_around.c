
/*!
 *************************************************************************************
 * \file quant4x4_around.c
 *
 * \brief
 *    Quantization process for a 4x4 block
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Alexis Michael Tourapis                  <alexismt@ieee.org>
 *    - Limin Liu                                <limin.liu@dolby.com>
 *
 *************************************************************************************
 */

#include "contributors.h"

#include <math.h>

#include "global.h"

#include "image.h"
#include "mb_access.h"
#include "vlc.h"
#include "transform.h"
#include "mc_prediction.h"
#include "q_offsets.h"
#include "q_matrix.h"
#include "quant4x4.h"

/*!
 ************************************************************************
 * \brief
 *   Quantization process for All coefficients for a 4x4 block
 *
 ************************************************************************
 */
int quant_4x4_around(Macroblock *currMB, int **tblock, struct quant_methods *q_method)
{
  VideoParameters *p_Vid = currMB->p_Vid;
  QuantParameters *p_Quant = p_Vid->p_Quant;
  Slice *currSlice = currMB->p_Slice;
  Boolean is_cavlc = (Boolean)(currSlice->symbol_mode == CAVLC);

  int AdaptRndWeight = p_Vid->AdaptRndWeight;

  int block_x = q_method->block_x;
  int qp = q_method->qp;
  int *ACL = &q_method->ACLevel[0];
  int *ACR = &q_method->ACRun[0];
  LevelQuantParams **q_params_4x4 = q_method->q_params;
  const byte(*pos_scan)[2] = q_method->pos_scan;
  const byte *c_cost = q_method->c_cost;
  int *coeff_cost = q_method->coeff_cost;

  LevelQuantParams *q_params = NULL;
  int **fadjust4x4 = q_method->fadjust;

  int i, j, coeff_ctr;

  int *m7;
  int scaled_coeff;

  int level, run = 0;
  int nonzero = FALSE;
  int qp_per = p_Quant->qp_per_matrix[qp];
  int q_bits = Q_BITS + qp_per;
  const byte *p_scan = &pos_scan[0][0];

  int *padjust4x4;

  // Quantization
  for (coeff_ctr = 0; coeff_ctr < 16; ++coeff_ctr)
  {
    i = *p_scan++; // horizontal position
    j = *p_scan++; // vertical position

    padjust4x4 = &fadjust4x4[j][block_x + i];
    m7 = &tblock[j][block_x + i];

    if (*m7 != 0)
    {
      q_params = &q_params_4x4[j][i];
      scaled_coeff = iabs(*m7) * q_params->ScaleComp;
      level = (scaled_coeff + q_params->OffsetComp) >> q_bits;

      if (level != 0)
      {
        if (is_cavlc)
          level = imin(level, CAVLC_LEVEL_LIMIT);

        *padjust4x4 = rshift_rnd_sf((AdaptRndWeight * (scaled_coeff - (level << q_bits))), q_bits + 1);

        *coeff_cost += (level > 1) ? MAX_VALUE : c_cost[run];

        level = isignab(level, *m7);
        *m7 = rshift_rnd_sf(((level * q_params->InvScaleComp) << qp_per), 4);
        // inverse scale can be alternative performed as follows to ensure 16bit
        // arithmetic is satisfied.
        // *m7 = (qp_per<4) ? rshift_rnd_sf((level*q_params->InvScaleComp),4-qp_per) : (level*q_params->InvScaleComp)<<(qp_per-4);
        *ACL++ = level;
        *ACR++ = run;
        // reset zero level counter
        run = 0;
        nonzero = TRUE;
      }
      else
      {
        *padjust4x4 = 0;
        *m7 = 0;
        ++run;
      }
    }
    else
    {
      *padjust4x4 = 0;
      ++run;
    }
  }

  *ACL = 0;

  return nonzero;
}

int quant_ac4x4_around(Macroblock *currMB, int **tblock, struct quant_methods *q_method)
{
  int block_x = q_method->block_x;

  int qp = q_method->qp;
  int *ACL = &q_method->ACLevel[0];
  int *ACR = &q_method->ACRun[0];
  LevelQuantParams **q_params_4x4 = q_method->q_params;
  int **fadjust4x4 = q_method->fadjust;
  const byte(*pos_scan)[2] = q_method->pos_scan;
  const byte *c_cost = q_method->c_cost;
  int *coeff_cost = q_method->coeff_cost;

  Boolean is_cavlc = (Boolean)(currMB->p_Slice->symbol_mode == CAVLC);
  VideoParameters *p_Vid = currMB->p_Vid;
  QuantParameters *p_Quant = p_Vid->p_Quant;
  int AdaptRndWeight = p_Vid->AdaptRndWeight;
  LevelQuantParams *q_params = NULL;

  int i, j, coeff_ctr;

  int *m7;
  int scaled_coeff;

  int level, run = 0;
  int nonzero = FALSE;
  int qp_per = p_Quant->qp_per_matrix[qp];
  int q_bits = Q_BITS + qp_per;
  const byte *p_scan = &pos_scan[1][0];
  int *padjust4x4;

  // Quantization
  for (coeff_ctr = 1; coeff_ctr < 16; ++coeff_ctr)
  {
    i = *p_scan++; // horizontal position
    j = *p_scan++; // vertical position

    padjust4x4 = &fadjust4x4[j][block_x + i];
    m7 = &tblock[j][block_x + i];
    if (*m7 != 0)
    {
      q_params = &q_params_4x4[j][i];
      scaled_coeff = iabs(*m7) * q_params->ScaleComp;
      level = (scaled_coeff + q_params->OffsetComp) >> q_bits;

      if (level != 0)
      {
        if (is_cavlc)
          level = imin(level, CAVLC_LEVEL_LIMIT);

        *padjust4x4 = rshift_rnd_sf((AdaptRndWeight * (scaled_coeff - (level << q_bits))), (q_bits + 1));

        *coeff_cost += (level > 1) ? MAX_VALUE : c_cost[run];

        level = isignab(level, *m7);
        *m7 = rshift_rnd_sf(((level * q_params->InvScaleComp) << qp_per), 4);
        // inverse scale can be alternative performed as follows to ensure 16bit
        // arithmetic is satisfied.
        // *m7 = (qp_per<4) ? rshift_rnd_sf((level*q_params->InvScaleComp),4-qp_per) : (level*q_params->InvScaleComp)<<(qp_per-4);
        *ACL++ = level;
        *ACR++ = run;
        // reset zero level counter
        run = 0;
        nonzero = TRUE;
      }
      else
      {
        *padjust4x4 = 0;
        *m7 = 0;
        ++run;
      }
    }
    else
    {
      *padjust4x4 = 0;
      ++run;
    }
  }

  *ACL = 0;

  return nonzero;
}

/*!
 ************************************************************************
 * \brief
 *    Quantization process for All coefficients for a 4x4 DC block
 *
 ************************************************************************
 */
int quant_dc4x4_around(Macroblock *currMB, int **tblock, int qp, int *DCLevel, int *DCRun,
                       LevelQuantParams *q_params_4x4, const byte (*pos_scan)[2])
{
  QuantParameters *p_Quant = currMB->p_Vid->p_Quant;
  Boolean is_cavlc = (Boolean)(currMB->p_Slice->symbol_mode == CAVLC);
  int i, j, coeff_ctr;

  int *m7;
  int scaled_coeff;

  int level, run = 0;
  int nonzero = FALSE;
  int qp_per = p_Quant->qp_per_matrix[qp];
  int q_bits = Q_BITS + qp_per + 1;
  const byte *p_scan = &pos_scan[0][0];
  int *DCL = &DCLevel[0];
  int *DCR = &DCRun[0];

  // Quantization
  for (coeff_ctr = 0; coeff_ctr < 16; ++coeff_ctr)
  {
    i = *p_scan++; // horizontal position
    j = *p_scan++; // vertical position

    m7 = &tblock[j][i];

    if (*m7 != 0)
    {
      scaled_coeff = iabs(*m7) * q_params_4x4->ScaleComp;
      level = (scaled_coeff + (q_params_4x4->OffsetComp << 1)) >> q_bits;

      if (level != 0)
      {
        if (is_cavlc)
          level = imin(level, CAVLC_LEVEL_LIMIT);

        level = isignab(level, *m7);
        *m7 = level;
        *DCL++ = level;
        *DCR++ = run;
        // reset zero level counter
        run = 0;
        nonzero = TRUE;
      }
      else
      {
        ++run;
        *m7 = 0;
      }
    }
    else
    {
      ++run;
    }
  }

  *DCL = 0;

  return nonzero;
}
