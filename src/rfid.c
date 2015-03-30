#include "ML674061.h"
#include "common.h"
#include "rfid.h"
#include "i2c.h"
#include "delay.h"
#include "uart.h"
#include "vlog.h"

int32_t i2cerror;

int32_t writecheck(uint8_t addr_i2c, uint8_t *data, uint8_t nb_byte)
{
  int32_t nmax=1000;
  while((nmax>0)&&(write_i2c(addr_i2c,data,nb_byte)==FALSE)){ nmax--;__no_operation();}
  if (!nmax) i2cerror=1;
  return nmax;
}

int32_t readcheck(uint8_t addr_i2c, uint8_t *data, uint8_t nb_byte)
{
  int32_t nmax=1000;
  while((nmax>0)&&(read_i2c(addr_i2c,data,nb_byte)==FALSE)){ nmax--;__no_operation();}
  if (!nmax) i2cerror=1;
  return nmax;
}

/****************************************************************************/
/*  Turn ON electromagnetic field and initialize CRX14                      */
/*  Function : init_rfid                                                    */
/*      Parameters                                                          */
/*          Input   :   Nothing                                             */
/*          Output  :   Return the completion of turning ON                 */
/****************************************************************************/
uint8_t init_rfid(void)
{
  uint8_t dummy_tab[2];
  //Turn OFF RF
  close_rfid();
  //Turn ON RF
  dummy_tab[0]=CRX14_PARAMETER_REGISTER;
  dummy_tab[1]=0x10;
  if(!writecheck(CRX14_ADDR,dummy_tab,2)) return FALSE;
  //Read answer tab
  if(!readcheck(CRX14_ADDR,dummy_tab,1)) return FALSE;
  //Check if module is ON
  if(dummy_tab[0]==0x10)
    return TRUE;
  else
    return FALSE;
}

/****************************************************************************/
/*  Turn OFF electromagnetic field                                          */
/*  Function : close_rfid                                                   */
/*      Parameters                                                          */
/*          Input   :   Nothing                                             */
/*          Output  :   Return the completion of turning OFF                */
/****************************************************************************/
uint8_t close_rfid(void)
{
  uint8_t dummy_tab[2];
  //Unselect tags
  completion_rfid();
  //Turn OFF RF
  dummy_tab[0]=CRX14_PARAMETER_REGISTER;
  dummy_tab[1]=0x00;
  if(!writecheck(CRX14_ADDR,dummy_tab,2)) return FALSE;
  //Read answer tab
  if(!readcheck(CRX14_ADDR,dummy_tab,1)) return FALSE;
  //Check if module is OFF
  if(dummy_tab[0]==0x00)
    return TRUE;
  else
    return FALSE;
}

/****************************************************************************/
/*  RFID command to initiate all TAGs present in the electromagnetic field  */
/*  Function : initiate_rfid                                                */
/*      Parameters                                                          */
/*          Input   :   Nothing                                             */
/*          Output  :   Nothing                                             */
/****************************************************************************/
uint8_t initiate_rfid(void)
{
  uint8_t dummy_tab[4];
  dummy_tab[0]=CRX14_IO_FRAME_REGISTER;
  dummy_tab[1]=0x02;
  dummy_tab[2]=0x06;
  dummy_tab[3]=0x00;
  if(!writecheck(CRX14_ADDR,dummy_tab,4)) return FALSE;
  return TRUE;
}

/****************************************************************************/
/*  RFID command to check all TAGs present in the electromagnetic field     */
/*  Function : slot_marker_rfid                                             */
/*      Parameters                                                          */
/*          Input   :   Nothing                                             */
/*          Output  :   Nothing                                             */
/****************************************************************************/
uint8_t slot_marker_rfid(void)
{
  uint8_t dummy_tab[4];
  dummy_tab[0]=CRX14_SLOT_MARKER_REGISTER;
  if(!writecheck(CRX14_ADDR,dummy_tab,1)) return FALSE;
  return TRUE;
}

/****************************************************************************/
/*  RFID command to select a TAG                                            */
/*  Function : completion_rfid                                              */
/*      Parameters                                                          */
/*          Input   :   CHIP_ID of the TAG to select                        */
/*          Output  :   Nothing                                             */
/****************************************************************************/
uint8_t select_tag_rfid(uint8_t chip_id)
{
  uint8_t dummy_tab[4];
  dummy_tab[0]=CRX14_IO_FRAME_REGISTER;
  dummy_tab[1]=0x02;
  dummy_tab[2]=0x0E;
  dummy_tab[3]=chip_id;
  if(!writecheck(CRX14_ADDR,dummy_tab,4)) return FALSE;
  return TRUE;

}

/****************************************************************************/
/*  RFID command to read frame buffer of CRX14                              */
/*  Function : read_frame_rfid                                              */
/*      Parameters                                                          */
/*          Input   :   pointer to the data to returned                     */
/*          Input   :   number of bytes to read                             */
/*          Output  :   Nothing                                             */
/****************************************************************************/
uint8_t read_frame_rfid(uint8_t *data, uint8_t nb_bytes)
{
  uint8_t dummy_tab[1];
  uint8_t dummy_cmpt;

  //Blank answer tab
  for(dummy_cmpt=0; dummy_cmpt<nb_bytes; dummy_cmpt++)
    *(data+dummy_cmpt)=0x00;
 //Select frame register
  dummy_tab[0]=CRX14_IO_FRAME_REGISTER;
  if(!writecheck(CRX14_ADDR,dummy_tab,1)) return FALSE;
  //Read answer tab
  if(!readcheck(CRX14_ADDR,data,nb_bytes)) return FALSE;
  return TRUE;
}

/****************************************************************************/
/*  RFID command to deactived TAGs                                          */
/*  Function : completion_rfid                                              */
/*      Parameters                                                          */
/*          Input   :   Nothing                                             */
/*          Output  :   Nothing                                             */
/****************************************************************************/
uint8_t completion_rfid(void)
{
  uint8_t dummy_tab[3];
  dummy_tab[0]=CRX14_IO_FRAME_REGISTER;
  dummy_tab[1]=0x01;
  dummy_tab[2]=0x0F;
  if(!writecheck(CRX14_ADDR,dummy_tab,3)) return FALSE;
  return TRUE;
}

/****************************************************************************/
/*  RFID command to get unique UID of selected TAG                          */
/*  Function : get_uid_rfid                                           */
/*      Parameters                                                          */
/*          Input   :   Nothing                                             */
/*          Output  :   Nothing                                             */
/****************************************************************************/
uint8_t get_uid_rfid(void)
{
  uint8_t dummy_tab[3];
  dummy_tab[0]=CRX14_IO_FRAME_REGISTER;
  dummy_tab[1]=0x01;
  dummy_tab[2]=0x0B;
  if(!writecheck(CRX14_ADDR,dummy_tab,3)) return FALSE;
  return TRUE;
}

/****************************************************************************/
/*  Check if RFID tags are present                                          */
/*  Function : check_rfid_devices                                           */
/*      Parameters                                                          */
/*          Input   :   Pointer to the structure containing for each tag    */
/*                     the CHIP_ID (1 byte, random) and UID (8 bytes, fixed)*/
/*          Output  :   Return the number of tags detected (maximum 16)     */
/****************************************************************************/
uint8_t check_rfid_devices(struct _tag_rfid *p_tag_rfid)
{
  uint8_t dummy_tab[20];
  uint16_t dummy_short;
  uint8_t dummy_cmpt;
  uint8_t cmpt_tags=0;

  //Turn ON RFID
  init_rfid();
  //Delay for RFID, no answer from device if ommited, due to delay after I2C stop I think
  DelayMs(1);

  //Run Initiate Command to tags
  initiate_rfid();
  //Delay for RFID, no answer from device if ommited, due to delay after I2C stop I think
  DelayMs(1);
  //Read reception buffer
  read_frame_rfid(dummy_tab, 2);

  //return if no tag detected
  if(dummy_tab[0]==0x00)
  {
    //Turn OFF RFID
    close_rfid();
    return FALSE;
  }

  //Turn on detected sequence of tags
  slot_marker_rfid();
  //Delay for RFID, no answer from device if ommited, due to delay after I2C stop I think
  DelayMs(1);
  //Read reception buffer
  read_frame_rfid(dummy_tab, 19);

  //Check detected devices => CHIP_ID
  dummy_short = (dummy_tab[2]<<8) + dummy_tab[1];
  for(dummy_cmpt=0; dummy_cmpt<16; dummy_cmpt++)
  {
    if(dummy_short & 0x0001)
    {
        (p_tag_rfid+cmpt_tags)->CHIP_ID=dummy_tab[dummy_cmpt+3];
        cmpt_tags++;
    }
    dummy_short>>=1;
  }

  //Get UID of detected tags
  for(dummy_cmpt=0; dummy_cmpt<cmpt_tags; dummy_cmpt++)
  {
    uint8_t dummy_cmpt_2;
    //Select Tag
    select_tag_rfid((p_tag_rfid+dummy_cmpt)->CHIP_ID);
    //Delay for RFID, no answer from device if ommited, due to delay after I2C stop I think
    DelayMs(1);
    //Read reception buffer
    read_frame_rfid(dummy_tab, 2);

    //Get UID of selected tag
    get_uid_rfid();
    //Delay for RFID, no answer from device if ommited, due to delay after I2C stop I think
    DelayMs(1);
    //Read reception buffer
    read_frame_rfid(dummy_tab, 9);

    //Copy UID to struct
    for(dummy_cmpt_2=0; dummy_cmpt_2<8; dummy_cmpt_2++)
      (p_tag_rfid+dummy_cmpt)->UID[dummy_cmpt_2]=dummy_tab[7-dummy_cmpt_2+1];

    __no_operation();
  }

  //return the number of tags detected
  return cmpt_tags;
}

/****************************************************************************/
/*  Write data in EEPROM of selected TAG                                    */
/*  Function : write_eeprom_rfid                                            */
/*      Parameters                                                          */
/*          Input   :   CHIP_ID of the TAG                                  */
/*          Input   :   Reference of the block where writing.               */
/*                      SRIX4K => 121 blocks from 0 to 120                  */
/*          Input   :   pointer to the data to write                        */
/*          Input   :   Number of bytes to write from 0 to 4                */
/****************************************************************************/
void write_eeprom_rfid(uint8_t chip_id, uint8_t num_block, uint8_t *data, uint8_t num_bytes)
{
  uint8_t dummy_tab[10];
  uint8_t dummy_cmpt;

  //Select Tag
  select_tag_rfid(chip_id);
  //Delay for RFID, no answer from device if ommited, due to delay after I2C stop I think
  DelayMs(1);

  //Prepare command
  dummy_tab[0]=CRX14_IO_FRAME_REGISTER;
  dummy_tab[1]=(uint8_t)(2+num_bytes);
  dummy_tab[2]=0x09;
  dummy_tab[3]=num_block+7;
  for(dummy_cmpt=0; dummy_cmpt<num_bytes; dummy_cmpt++)
    dummy_tab[dummy_cmpt+4]=*(data+dummy_cmpt);
  //Send command
  while(write_i2c(CRX14_ADDR,dummy_tab,(uint8_t)(4+num_bytes))==FALSE)
    __no_operation();
}

/****************************************************************************/
/*  Read data in EEPROM of selected TAG                                     */
/*  Function : read_eeprom_rfid                                             */
/*      Parameters                                                          */
/*          Input   :   CHIP_ID of the TAG                                  */
/*          Input   :   Reference of the block where reading.               */
/*                      SRIX4K => 121 blocks from 0 to 120                  */
/*          Input   :   pointer to the returned data                        */
/*          Input   :   Number of bytes to read from 0 to 4                 */
/****************************************************************************/
void read_eeprom_rfid(uint8_t chip_id, uint8_t num_block, uint8_t *data, uint8_t num_bytes)
{
  uint8_t dummy_tab[10];
  uint8_t dummy_cmpt;

  //Select Tag
  select_tag_rfid(chip_id);
  //Delay for RFID, no answer from device if ommited, due to delay after I2C stop I think
  DelayMs(1);

  //Prepare command
  dummy_tab[0]=CRX14_IO_FRAME_REGISTER;
  dummy_tab[1]=0x02;
  dummy_tab[2]=0x08;
  dummy_tab[3]=num_block+7;
  //Send command
  while(write_i2c(CRX14_ADDR,dummy_tab,4)==FALSE)
    __no_operation();

  //Delay for RFID, no answer from device if ommited, due to delay after I2C stop I think
  DelayMs(1);
  //Read reception buffer
  read_frame_rfid(dummy_tab, 5);

  //Copy answer
  for(dummy_cmpt=0; dummy_cmpt<num_bytes; dummy_cmpt++)
    *(data+dummy_cmpt)=dummy_tab[dummy_cmpt+1];
}


//Maximum of 16 tag for CRX14
struct _tag_rfid tag_rfid[16];
uint8_t nb_tag_detected=0;

uint8_t* get_rfid_first_device()
{
  i2cerror=0;
  nb_tag_detected=check_rfid_devices(tag_rfid);
  close_rfid();
  if (i2cerror) return (uint8_t*)"Error";
  if(nb_tag_detected) return (uint8_t*)&(tag_rfid[0].UID[0]);
  return NULL;
}

int32_t check_rfid_n()
{
  i2cerror=0;
  nb_tag_detected=check_rfid_devices(tag_rfid);
  close_rfid();
  if (i2cerror) return -1;
  return nb_tag_detected;
}

uint8_t* get_nth_rfid(int32_t i)
{
  return (uint8_t*)&(tag_rfid[i].UID[0]);
}

int32_t checkid(uint8_t* p, uint8_t* q,int32_t n)
{
  int32_t i;
  for(i=0;i<n;i++) if (p[i]!=q[i]) return 1;
  return 0;
}

int32_t get_byuid(uint8_t* id)
{
  int32_t i;
  for(i=0;i<nb_tag_detected;i++)
  {
//    dump((uint8_t*)&(tag_rfid[i].UID[0]),8);
    if (!checkid(id,(uint8_t*)&(tag_rfid[i].UID[0]),8))
    {
      consolestr("found !\n");
      return tag_rfid[i].CHIP_ID;
    }
  }
  return -1;
}

int32_t rfid_read(uint8_t* id,int32_t bloc,uint8_t* data)
{
  consolestr("rfid_read\n");
//  dump(id,8);
  i2cerror=0;
//  putst_uart("detect\n");
  nb_tag_detected=check_rfid_devices(tag_rfid);
//  putst_uart("nb_tag_detected=");puthx_uart(nb_tag_detected);putst_uart("\n");
  if (i2cerror)
  {
    close_rfid();
    return -1;
  }
  int32_t chipid=get_byuid(id);
  consolestr("chipid=");consolehx(chipid);consolestr("\n");
  if (chipid==-1)
  {
    close_rfid();
    return -1;
  }
//  putst_uart("read\n");
  read_eeprom_rfid(chipid, bloc, data, 4);
//  dump(data,4);
  close_rfid();
  return 0;
}
int32_t rfid_write(uint8_t* id,int32_t bloc,uint8_t* data)
{
  i2cerror=0;
  nb_tag_detected=check_rfid_devices(tag_rfid);
  if (i2cerror)
  {
    close_rfid();
    return -1;
  }
  int32_t chipid=get_byuid(id);
  if (chipid==-1)
  {
    close_rfid();
    return -1;
  }
  write_eeprom_rfid(chipid, bloc, data, 4);
  close_rfid();
  return 0;
}
/*
  while(1)
  {
    uint8_t nb_tag_detected;
    uint8_t tab_rfid[4];

    //Check if tag are present
    //if yes get CHIP_ID (1byte) and UID (8bytes) in structure
    nb_tag_detected=check_rfid_devices(tag_rfid);
    if(nb_tag_detected)
    {
      //Prepare tab
      tab_rfid[0]=0x12;
      tab_rfid[1]=0x34;
      tab_rfid[2]=0x56;
      tab_rfid[3]=0x78;

      //Write data in a specified block, 121 blocks of 4bytes
      write_eeprom_rfid(tag_rfid[0].CHIP_ID, 0,tab_rfid, 4);

      //Delay for EEPROM technology write
      DelayMs(5);

      //Clear tab
      tab_rfid[0]=tab_rfid[1]=tab_rfid[2]=tab_rfid[3]=0;

      //Read data from a specified block, 121 blocks of 4bytes
      read_eeprom_rfid(tag_rfid[0].CHIP_ID, 0, tab_rfid, 4);

      //Turn OFF RFID
      close_rfid();
    }
    DelayMs(100);
  }


*/