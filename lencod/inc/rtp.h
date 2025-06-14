
/*!
 ***************************************************************************
 *
 * \file rtp.h
 *
 * \brief
 *    Definition of structures and functions to handle RTP headers.  For a
 *    description of RTP see RFC1889 on http://www.ietf.org
 *
 * \date
 *    30 September 2001
 *
 * \author
 *    Stephan Wenger   stewe@cs.tu-berlin.de
 **************************************************************************/

#ifndef _RTP_H_
#define _RTP_H_

#include "nalu.h"

#define MAXRTPPAYLOADLEN (65536 - 40) //!< Maximum payload size of an RTP packet
#define MAXRTPPACKETSIZE (65536 - 28) //!< Maximum size of an RTP packet incl. header
#define H264PAYLOADTYPE 105           //!< RTP paylaod type fixed here for simplicity
#define H264SSRC 0x12345678           //!< SSRC, chosen to simplify debugging
#define RTP_TR_TIMESTAMP_MULT 1000    //!< should be something like 27 Mhz / 29.97 Hz

typedef struct
{
  unsigned int v;         //!< Version, 2 bits, MUST be 0x2
  unsigned int p;         //!< Padding bit, Padding MUST NOT be used
  unsigned int x;         //!< Extension, MUST be zero
  unsigned int cc;        /*!< CSRC count, normally 0 in the absence
                               of RTP mixers */
  unsigned int m;         //!< Marker bit
  unsigned int pt;        //!< 7 bits, Payload Type, dynamically established
  unsigned int seq;       /*!< RTP sequence number, incremented by one for
                               each sent packet */
  unsigned int timestamp; //!< timestamp, 27 MHz for H.264
  unsigned int ssrc;      //!< Synchronization Source, chosen randomly
  byte *payload;          //!< the payload including payload headers
  unsigned int paylen;    //!< length of payload in bytes
  byte *packet;           //!< complete packet including header and payload
  unsigned int packlen;   //!< length of packet, typically paylen+12
} RTPpacket_t;

extern void RTPUpdateTimestamp(VideoParameters *p_Vid, int tr);
extern void OpenRTPFile(char *Filename, FILE **f_rtp);
extern void CloseRTPFile(FILE *f_rtp);
extern int WriteRTPNALU(VideoParameters *p_Vid, NALU_t *n, FILE **f_rtp);

#endif
