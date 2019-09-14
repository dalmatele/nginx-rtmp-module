#include <ngx_config.h>
#include <ngx_core.h>
#include "ngx_rtmp_fmp4.h"
#include <ngx_rtmp_codec_module.h>


 ngx_int_t
ngx_rtmp_fmp4_field_32(ngx_buf_t *b, uint32_t n)
{
    u_char  bytes[4];

    bytes[0] = ((uint32_t) n >> 24) & 0xFF;
    bytes[1] = ((uint32_t) n >> 16) & 0xFF;
    bytes[2] = ((uint32_t) n >> 8) & 0xFF;
    bytes[3] = (uint32_t) n & 0xFF;

    if (b->last + sizeof(bytes) > b->end) {
        return NGX_ERROR;
    }

    b->last = ngx_cpymem(b->last, bytes, sizeof(bytes));

    return NGX_OK;
}


 ngx_int_t
ngx_rtmp_fmp4_field_24(ngx_buf_t *b, uint32_t n)
{
    u_char  bytes[3];

    bytes[0] = ((uint32_t) n >> 16) & 0xFF;
    bytes[1] = ((uint32_t) n >> 8) & 0xFF;
    bytes[2] = (uint32_t) n & 0xFF;

    if (b->last + sizeof(bytes) > b->end) {
        return NGX_ERROR;
    }

    b->last = ngx_cpymem(b->last, bytes, sizeof(bytes));

    return NGX_OK;
}


 ngx_int_t
ngx_rtmp_fmp4_field_16(ngx_buf_t *b, uint16_t n)
{
    u_char  bytes[2];

    bytes[0] = ((uint32_t) n >> 8) & 0xFF;
    bytes[1] = (uint32_t) n & 0xFF;

    if (b->last + sizeof(bytes) > b->end) {
        return NGX_ERROR;
    }

    b->last = ngx_cpymem(b->last, bytes, sizeof(bytes));

    return NGX_OK;
}


 ngx_int_t
ngx_rtmp_fmp4_field_8(ngx_buf_t *b, uint8_t n)
{
    u_char  bytes[1];

    bytes[0] = n & 0xFF;

    if (b->last + sizeof(bytes) > b->end) {
        return NGX_ERROR;
    }

    b->last = ngx_cpymem(b->last, bytes, sizeof(bytes));

    return NGX_OK;
}

ngx_int_t
ngx_rtmp_fmp4_write_ftyp(ngx_buf_t *b){
    u_char  *pos;
    pos = ngx_rtmp_fmp4_start_box(b, "ftyp");
    ngx_rtmp_fmp4_field_32(b, 0);
    //major brand
    ngx_rtmp_fmp4_box(b, "iso5");
    ngx_rtmp_fmp4_box(b, "512");
    //compatible brands:
    ngx_rtmp_fmp4_box(b, "iso6");
    ngx_rtmp_fmp4_box(b, "mp41");

    ngx_rtmp_fmp4_update_box_size(b, pos);

    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_mdhd(ngx_buf_t *b){
    u_char  *pos;
    pos = ngx_rtmp_fmp4_start_box(b, "mdhd");
    /* version */
    ngx_rtmp_fmp4_field_32(b, 0);
    /*box flags*/
    ngx_rtmp_fmp4_field_32(b, 0);

    /* creation time */
    ngx_rtmp_fmp4_field_32(b, 0);

    /* modification time */
    ngx_rtmp_fmp4_field_32(b, 0);

    /* time scale*/
    ngx_rtmp_fmp4_field_32(b, 1000);

    /* duration */
    ngx_rtmp_fmp4_field_32(b, 0);
    ngx_rtmp_fmp4_update_box_size(b, pos);
    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_hdlr(ngx_buf_t *b, int isVideo){
    u_char  *pos;

    pos = ngx_rtmp_fmp4_start_box(b, "hdlr");

    /* version and flags */
    ngx_rtmp_fmp4_field_32(b, 0);

    /* pre defined */
    ngx_rtmp_fmp4_field_32(b, 0);

    if (isVideo == 0) {
        ngx_rtmp_fmp4_box(b, "vide");
    } else {
        ngx_rtmp_fmp4_box(b, "soun");
    }

    /* reserved */
    ngx_rtmp_fmp4_field_32(b, 0);
    ngx_rtmp_fmp4_field_32(b, 0);
    ngx_rtmp_fmp4_field_32(b, 0);

    if (isVideo == 0) {
        /* video handler string, NULL-terminated */
        ngx_rtmp_fmp4_data(b, "Uiza Video Hanlder", sizeof("Uiza Video Hanlder"));
    } else {
        /* sound handler string, NULL-terminated */
        ngx_rtmp_fmp4_data(b, "Uiza Audio Handler", sizeof("Uiza Audio Hanlder"));
    }

    ngx_rtmp_fmp4_update_box_size(b, pos);

    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_data(ngx_buf_t *b, void *data, size_t n)
{
    if (b->last + n > b->end) {
        return NGX_ERROR;
    }

    b->last = ngx_cpymem(b->last, (u_char *) data, n);

    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_vmhd(ngx_buf_t *b)
{
    /* size is always 20, apparently */
    ngx_rtmp_fmp4_field_32(b, 20);

    ngx_rtmp_fmp4_box(b, "vmhd");

    /* version and flags */
    ngx_rtmp_fmp4_field_32(b, 0x01);

    /* reserved (graphics mode=copy) */
    ngx_rtmp_fmp4_field_32(b, 0);
    ngx_rtmp_fmp4_field_32(b, 0);

    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_smhd(ngx_buf_t *b)
{
    /* size is always 16, apparently */
    ngx_rtmp_fmp4_field_32(b, 16);

    ngx_rtmp_fmp4_box(b, "smhd");

    /* version and flags */
    ngx_rtmp_fmp4_field_32(b, 0);

    /* reserved (balance normally=0) */
    ngx_rtmp_fmp4_field_16(b, 0);
    ngx_rtmp_fmp4_field_16(b, 0);

    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_dref(ngx_buf_t *b)
{
    u_char  *pos;

    pos = ngx_rtmp_fmp4_start_box(b, "dref");

    /* version and flags */
    ngx_rtmp_fmp4_field_32(b, 0);

    /* entry count */
    ngx_rtmp_fmp4_field_32(b, 1);

    /* url size */
    ngx_rtmp_fmp4_field_32(b, 0xc);

    ngx_rtmp_fmp4_box(b, "url ");

    /* version and flags */
    ngx_rtmp_fmp4_field_32(b, 0x00000001);

    ngx_rtmp_fmp4_update_box_size(b, pos);

    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_dinf(ngx_buf_t *b)
{
    u_char  *pos;

    pos = ngx_rtmp_fmp4_start_box(b, "dinf");

    ngx_rtmp_fmp4_write_dref(b);

    ngx_rtmp_fmp4_update_box_size(b, pos);

    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_avc(ngx_rtmp_session_t *s, ngx_buf_t *b)
{
    u_char                *pos;
    ngx_rtmp_codec_ctx_t  *codec_ctx;

    codec_ctx = ngx_rtmp_get_module_ctx(s, ngx_rtmp_codec_module);

    pos = ngx_rtmp_fmp4_start_box(b, "avc1");

    /* reserved */
    ngx_rtmp_fmp4_field_32(b, 0);
    ngx_rtmp_fmp4_field_16(b, 0);

    /* data reference index */
    ngx_rtmp_fmp4_field_16(b, 1);

    /* codec stream version & revision */
    ngx_rtmp_fmp4_field_16(b, 0);
    ngx_rtmp_fmp4_field_16(b, 0);

    /* reserved */
    ngx_rtmp_fmp4_field_32(b, 0);
    ngx_rtmp_fmp4_field_32(b, 0);
    ngx_rtmp_fmp4_field_32(b, 0);

    /* width & height */
    ngx_rtmp_fmp4_field_16(b, (uint16_t) codec_ctx->width);
    ngx_rtmp_fmp4_field_16(b, (uint16_t) codec_ctx->height);

    /* horizontal & vertical resolutions 72 dpi */
    ngx_rtmp_fmp4_field_32(b, 0x00480000);
    ngx_rtmp_fmp4_field_32(b, 0x00480000);

    /* data size */
    ngx_rtmp_fmp4_field_32(b, 0);

    /* frame count */
    ngx_rtmp_fmp4_field_16(b, 1);

    /* compressor name */
    ngx_rtmp_fmp4_field_32(b, 0);
    ngx_rtmp_fmp4_field_32(b, 0);
    ngx_rtmp_fmp4_field_32(b, 0);

    /* reserved */
    ngx_rtmp_fmp4_field_32(b, 0);
    ngx_rtmp_fmp4_field_32(b, 0);
    ngx_rtmp_fmp4_field_32(b, 0);
    ngx_rtmp_fmp4_field_32(b, 0);
    ngx_rtmp_fmp4_field_32(b, 0);
    ngx_rtmp_fmp4_field_16(b, 0x18);
    ngx_rtmp_fmp4_field_16(b, 0xffff);

    ngx_rtmp_fmp4_write_avcc(s, b);

    ngx_rtmp_fmp4_update_box_size(b, pos);

    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_mp4a(ngx_rtmp_session_t *s, ngx_buf_t *b)
{
    u_char                *pos;
    ngx_rtmp_codec_ctx_t  *codec_ctx;

    codec_ctx = ngx_rtmp_get_module_ctx(s, ngx_rtmp_codec_module);

    pos = ngx_rtmp_fmp4_start_box(b, "mp4a");

    /* reserved */
    ngx_rtmp_fmp4_field_32(b, 0);
    ngx_rtmp_fmp4_field_16(b, 0);

    /* data reference index */
    ngx_rtmp_fmp4_field_16(b, 1);

    /* reserved */
    ngx_rtmp_fmp4_field_32(b, 0);
    ngx_rtmp_fmp4_field_32(b, 0);

    /* channel count */
    ngx_rtmp_fmp4_field_16(b, (uint16_t) codec_ctx->audio_channels);

    /* sample size */
    ngx_rtmp_fmp4_field_16(b, (uint16_t) (codec_ctx->sample_size * 8));

    /* reserved */
    ngx_rtmp_fmp4_field_32(b, 0);

    /* time scale */
    ngx_rtmp_fmp4_field_16(b, 1000);

    /* sample rate */
    ngx_rtmp_fmp4_field_16(b, (uint16_t) codec_ctx->sample_rate);

    ngx_rtmp_fmp4_write_esds(s, b);
#if 0
    /* tag size*/
    ngx_rtmp_mp4_field_32(b, 8);

    /* null tag */
    ngx_rtmp_mp4_field_32(b, 0);
#endif
    ngx_rtmp_fmp4_update_box_size(b, pos);

    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_esds(ngx_rtmp_session_t *s, ngx_buf_t *b)
{
    size_t                 dsi_len;
    u_char                *pos, *dsi;
    ngx_buf_t             *db;
    ngx_rtmp_codec_ctx_t  *codec_ctx;

    codec_ctx = ngx_rtmp_get_module_ctx(s, ngx_rtmp_codec_module);

    if (codec_ctx == NULL || codec_ctx->aac_header == NULL) {
        return NGX_ERROR;
    }

    db = codec_ctx->aac_header->buf;
    if (db == NULL) {
        return NGX_ERROR;
    }

    dsi = db->pos + 2;
    if (dsi > db->last) {
        return NGX_ERROR;
    }

    dsi_len = db->last - dsi;

    pos = ngx_rtmp_fmp4_start_box(b, "esds");

    /* version */
    ngx_rtmp_fmp4_field_32(b, 0);


    /* ES Descriptor */

    ngx_rtmp_fmp4_put_descr(b, 0x03, 23 + dsi_len);

    /* ES_ID */
    ngx_rtmp_fmp4_field_16(b, 1);

    /* flags */
    ngx_rtmp_fmp4_field_8(b, 0);


    /* DecoderConfig Descriptor */

    ngx_rtmp_fmp4_put_descr(b, 0x04, 15 + dsi_len);

    /* objectTypeIndication: Audio ISO/IEC 14496-3 (AAC) */
    ngx_rtmp_fmp4_field_8(b, 0x40);

    /* streamType: AudioStream */
    ngx_rtmp_fmp4_field_8(b, 0x15);

    /* bufferSizeDB */
    ngx_rtmp_fmp4_field_24(b, 0);

    /* maxBitrate */
    ngx_rtmp_fmp4_field_32(b, 0x0001F151);

    /* avgBitrate */
    ngx_rtmp_fmp4_field_32(b, 0x0001F14D);


    /* DecoderSpecificInfo Descriptor */

    ngx_rtmp_fmp4_put_descr(b, 0x05, dsi_len);
    ngx_rtmp_fmp4_data(b, dsi, dsi_len);


    /* SL Descriptor */

    ngx_rtmp_fmp4_put_descr(b, 0x06, 1);
    ngx_rtmp_fmp4_field_8(b, 0x02);

    ngx_rtmp_fmp4_update_box_size(b, pos);

    return NGX_OK;
}



 ngx_int_t
ngx_rtmp_fmp4_write_stsd(ngx_rtmp_session_t *s, ngx_buf_t *b, int isVideo)
{
    u_char  *pos;

    pos = ngx_rtmp_fmp4_start_box(b, "stsd");

    /* version & flags */
    ngx_rtmp_fmp4_field_32(b, 0);

    /* entry count */
    ngx_rtmp_fmp4_field_32(b, 1);

    if (isVideo == 0) {
        ngx_rtmp_fmp4_write_avc(s, b);
    } else {
        ngx_rtmp_fmp4_write_mp4a(s, b);
    }

    ngx_rtmp_fmp4_update_box_size(b, pos);

    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_stsc(ngx_buf_t *b)
{
    u_char  *pos;

    pos = ngx_rtmp_fmp4_start_box(b, "stsc");

    ngx_rtmp_fmp4_field_32(b, 0); /* version */
    ngx_rtmp_fmp4_field_32(b, 0); /* entry count */

    ngx_rtmp_fmp4_update_box_size(b, pos);

    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_stts(ngx_buf_t *b)
{
    u_char  *pos;

    pos = ngx_rtmp_fmp4_start_box(b, "stts");

    ngx_rtmp_fmp4_field_32(b, 0); /* version */
    ngx_rtmp_fmp4_field_32(b, 0); /* entry count */

    ngx_rtmp_fmp4_update_box_size(b, pos);

    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_stsz(ngx_buf_t *b)
{
    u_char  *pos;

    pos = ngx_rtmp_fmp4_start_box(b, "stsz");

    ngx_rtmp_fmp4_field_32(b, 0); /* version */
    ngx_rtmp_fmp4_field_32(b, 0); /* entry count */
    ngx_rtmp_fmp4_field_32(b, 0); /* moar zeros */

    ngx_rtmp_fmp4_update_box_size(b, pos);

    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_stco(ngx_buf_t *b)
{
    u_char  *pos;

    pos = ngx_rtmp_fmp4_start_box(b, "stco");

    ngx_rtmp_fmp4_field_32(b, 0); /* version */
    ngx_rtmp_fmp4_field_32(b, 0); /* entry count */

    ngx_rtmp_fmp4_update_box_size(b, pos);

    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_stbl(ngx_rtmp_session_t *s, ngx_buf_t *b, int isVideo){
    u_char  *pos;

    pos = ngx_rtmp_fmp4_start_box(b, "stbl");

    ngx_rtmp_fmp4_write_stsd(s, b, isVideo);
    ngx_rtmp_fmp4_write_stts(b);
    ngx_rtmp_fmp4_write_stsc(b);
    ngx_rtmp_fmp4_write_stsz(b);
    ngx_rtmp_fmp4_write_stco(b);

    ngx_rtmp_fmp4_update_box_size(b, pos);

    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_minf(ngx_rtmp_session_t *s, ngx_buf_t *b, int isVideo){
    u_char  *pos;

    pos = ngx_rtmp_fmp4_start_box(b, "minf");

    if (isVideo == 0) {
        ngx_rtmp_fmp4_write_vmhd(b);
    } else {
        ngx_rtmp_fmp4_write_smhd(b);
    }

    ngx_rtmp_fmp4_write_dinf(b);
    ngx_rtmp_fmp4_write_stbl(s, b, isVideo);

    ngx_rtmp_fmp4_update_box_size(b, pos);

    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_mdia(ngx_rtmp_session_t *s, ngx_buf_t *b,int isVideo){
    u_char  *pos;
    pos = ngx_rtmp_fmp4_start_box(b, "mdia");
    //write mdhd box
    ngx_rtmp_fmp4_write_mdhd(b);
    //write hdlr box
    ngx_rtmp_fmp4_write_hdlr(b, isVideo);
    //write minf box
    ngx_rtmp_fmp4_write_minf(s, b, isVideo);
    ngx_rtmp_fmp4_update_box_size(b, pos);
    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_trak(ngx_rtmp_session_t *s, ngx_buf_t *b, int isVideo){
    u_char  *pos;
    pos = ngx_rtmp_fmp4_start_box(b, "trak");
    ngx_rtmp_fmp4_write_tkhd(s, b, isVideo);
    //write edts
    ngx_rtmp_fmp4_write_mdia(s, b, isVideo);
    ngx_rtmp_fmp4_update_box_size(b, pos);
    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_tkhd(ngx_rtmp_session_t *s, ngx_buf_t *b, int isVideo){
    u_char  *pos;

    ngx_rtmp_codec_ctx_t  *codec_ctx;

    codec_ctx = ngx_rtmp_get_module_ctx(s, ngx_rtmp_codec_module);

    pos = ngx_rtmp_fmp4_start_box(b, "tkhd");
    /* version */
    ngx_rtmp_fmp4_field_8(b, 0);
    //flag
    ngx_rtmp_fmp4_field_24(b, 0x0000000f);
    /* creation time */
    ngx_rtmp_fmp4_field_32(b, 0);

    /* modification time */
    ngx_rtmp_fmp4_field_32(b, 0);
     /* track id */
    ngx_rtmp_fmp4_field_32(b, 1);

    /* reserved */
    ngx_rtmp_fmp4_field_32(b, 0);

    /* duration */
    ngx_rtmp_fmp4_field_32(b, 0);
    /* reserved */
    ngx_rtmp_fmp4_field_32(b, 0);
    ngx_rtmp_fmp4_field_32(b, 0);
    ngx_rtmp_fmp4_field_32(b, 0);

    /* reserved */
    ngx_rtmp_fmp4_field_16(b, isVideo == 1 ?  0 : 0x0100);

    /* reserved */
    ngx_rtmp_fmp4_field_16(b, 0);

    ngx_rtmp_fmp4_write_matrix(b, 1, 0, 0, 1, 0, 0);

    if (isVideo == 0) {
        ngx_rtmp_fmp4_field_32(b, (uint32_t) codec_ctx->width << 16);
        ngx_rtmp_fmp4_field_32(b, (uint32_t) codec_ctx->height << 16);
    } else {
        ngx_rtmp_fmp4_field_32(b, 0);
        ngx_rtmp_fmp4_field_32(b, 0);
    }

    ngx_rtmp_fmp4_update_box_size(b, pos);
    return NGX_OK;
}

ngx_int_t
ngx_rtmp_fmp4_write_moov(ngx_rtmp_session_t *s, ngx_buf_t *b){
    u_char  *pos;
    pos = ngx_rtmp_fmp4_start_box(b, "moov");
    ngx_rtmp_fmp4_write_mvhd(b);
    ngx_rtmp_fmp4_write_mvex(b);
    ngx_rtmp_fmp4_write_trak(s, b, 0);//video
    ngx_rtmp_fmp4_write_trak(s, b, 1);//audio
    ngx_rtmp_fmp4_update_box_size(b, pos);

    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_mvex(ngx_buf_t *b){
    u_char  *pos;
    pos = ngx_rtmp_fmp4_start_box(b, "mvex");
    ngx_rtmp_fmp4_field_32(b, 0x20);//seperate between box
    ngx_rtmp_fmp4_box(b, "trex");
    /* version & flags */
    ngx_rtmp_fmp4_field_32(b, 0);
    ngx_rtmp_fmp4_field_32(b, 0);
    /* track id */
    ngx_rtmp_fmp4_field_32(b, 1);
    /* default sample description index */
    ngx_rtmp_fmp4_field_32(b, 1);
    /* default sample duration */
    ngx_rtmp_fmp4_field_32(b, 0);

    /* default sample size, 1024 for AAC */
    ngx_rtmp_fmp4_field_32(b, 0);

    ngx_rtmp_fmp4_field_32(b, 0x20);//seperate between box
    ngx_rtmp_fmp4_box(b, "trex");
    /* version & flags */
    ngx_rtmp_fmp4_field_32(b, 0);
    ngx_rtmp_fmp4_field_32(b, 0);
    /* track id */
    ngx_rtmp_fmp4_field_32(b, 2);
    /* default sample description index */
    ngx_rtmp_fmp4_field_32(b, 1);
    /* default sample duration */
    ngx_rtmp_fmp4_field_32(b, 0);

    /* default sample size, 1024 for AAC */
    ngx_rtmp_fmp4_field_32(b, 0);
    /* default sample flags, key on */
    ngx_rtmp_fmp4_field_32(b, 0);



    ngx_rtmp_fmp4_update_box_size(b, pos);
    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_mvhd(ngx_buf_t *b){
    u_char  *pos;
    pos = ngx_rtmp_fmp4_start_box(b, "mvhd");
    //box version
    ngx_rtmp_fmp4_box(b, 0);
    //box flags
    ngx_rtmp_fmp4_box(b, 0);
    //create time
    ngx_rtmp_fmp4_box(b, 0);
    //modification time
    ngx_rtmp_fmp4_box(b, 0);
    //timescale.
    //FIXME: why 1000?
    ngx_rtmp_fmp4_field_32(b, 1000);
    //duration
    ngx_rtmp_fmp4_field_32(b, 0);
    ngx_rtmp_fmp4_update_box_size(b, pos);
    return NGX_OK;
}

 u_char *
ngx_rtmp_fmp4_start_box(ngx_buf_t *b, const char box[4]){
    u_char  *p;

    p = b->last;

    if (ngx_rtmp_fmp4_field_32(b, 0) != NGX_OK) {
        return NULL;
    }

    if (ngx_rtmp_fmp4_box(b, box) != NGX_OK) {
        return NULL;
    }

    return p;
}

 ngx_int_t
ngx_rtmp_fmp4_write_matrix(ngx_buf_t *buf, uint32_t a, uint32_t b, uint32_t c,
    uint32_t d, uint32_t tx, uint32_t ty)
{

/*
 * transformation matrix
 * |a  b  u|
 * |c  d  v|
 * |tx ty w|
 */

    ngx_rtmp_fmp4_field_32(buf, a << 16);  /* 16.16 format */
    ngx_rtmp_fmp4_field_32(buf, b << 16);  /* 16.16 format */
    ngx_rtmp_fmp4_field_32(buf, 0);        /* u in 2.30 format */
    ngx_rtmp_fmp4_field_32(buf, c << 16);  /* 16.16 format */
    ngx_rtmp_fmp4_field_32(buf, d << 16);  /* 16.16 format */
    ngx_rtmp_fmp4_field_32(buf, 0);        /* v in 2.30 format */
    ngx_rtmp_fmp4_field_32(buf, tx << 16); /* 16.16 format */
    ngx_rtmp_fmp4_field_32(buf, ty << 16); /* 16.16 format */
    ngx_rtmp_fmp4_field_32(buf, 1 << 30);  /* w in 2.30 format */

    return NGX_OK;
}

/**
 * Create a 4-byte field
 * */
 ngx_int_t
ngx_rtmp_fmp4_box(ngx_buf_t *b, const char box[4]){
    if (b->last + 4 > b->end) {
        return NGX_ERROR;
    }

    b->last = ngx_cpymem(b->last, (u_char *) box, 4);

    return NGX_OK;
}

//update b->last to the latest byte of memory
 ngx_int_t
ngx_rtmp_fmp4_update_box_size(ngx_buf_t *b, u_char *p){
    u_char  *curpos;

    if (p == NULL) {
        return NGX_ERROR;
    }

    curpos = b->last;

    b->last = p;

    ngx_rtmp_fmp4_field_32(b, (uint32_t) (curpos - p));

    b->last = curpos;

    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_meta(ngx_buf_t *b){
    u_char  *pos;
    pos = ngx_rtmp_fmp4_start_box(b, "meta");
    ngx_rtmp_fmp4_field_32(b, 0); /* version */
    //we use this to write some extend metadata, so keep reserved for future
    ngx_rtmp_fmp4_write_udta(b);
    ngx_rtmp_fmp4_update_box_size(b, pos);

    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_udta(ngx_buf_t *b){
    u_char  *pos;
    pos = ngx_rtmp_fmp4_start_box(b, "udta");
    ngx_rtmp_fmp4_write_meta(b);
    //write meta box

    ngx_rtmp_fmp4_update_box_size(b, pos);

    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_write_avcc(ngx_rtmp_session_t *s, ngx_buf_t *b){
    u_char                *pos, *p;
    ngx_chain_t           *in;
    ngx_rtmp_codec_ctx_t  *codec_ctx;

    codec_ctx = ngx_rtmp_get_module_ctx(s, ngx_rtmp_codec_module);

    if (codec_ctx == NULL) {
        return NGX_ERROR;
    }

    in = codec_ctx->avc_header;
    if (in == NULL) {
        return NGX_ERROR;
    }

    pos = ngx_rtmp_fmp4_start_box(b, "avcC");

    /* assume config fits one chunk (highly probable) */

    /*
     * Skip:
     * - flv fmt
     * - H264 CONF/PICT (0x00)
     * - 0
     * - 0
     * - 0
     */

    p = in->buf->pos + 5;

    if (p < in->buf->last) {
        ngx_rtmp_fmp4_data(b, p, (size_t) (in->buf->last - p));
    } else {
        ngx_log_error(NGX_LOG_ERR, s->connection->log, ngx_errno,
                      "dash: invalid avcc received");
    }

    ngx_rtmp_fmp4_update_box_size(b, pos);

    return NGX_OK;
}

 ngx_int_t
ngx_rtmp_fmp4_put_descr(ngx_buf_t *b, int tag, size_t size){
    ngx_rtmp_fmp4_field_8(b, (uint8_t) tag);
    ngx_rtmp_fmp4_field_8(b, size & 0x7F);

    return NGX_OK;
}
