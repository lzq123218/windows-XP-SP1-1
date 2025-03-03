rem
rem    This generates pooltag.txt
rem
rem    This module contains the tags used for various pool allocations by
rem    all kernel mode components.
rem
rem    This module is directly copied on a periodic basis and made available
rem    to both internal developers and external customers via a Knowledge Base
rem    article.
rem
rem    Due to the very wide distribution of this list (most of whom will not
rem    have access to source code) please make your tag description as clear
rem    as possible.
rem
rem    For debugging purposes please keep the file in following format:
rem       <PoolTag> - <Description text> [- <binary-name>] [- OWNER <owner>]
rem

8042 - PS/2 kb and mouse - i8042prt.sys
ARPC - ATM ARP Client - atmarpc.sys
ATMU - ATM UNI Call Manager - atmuni.sys
Abos - Abiosdsk
AcdM - TDI AcdObjectInfoG
AcdN - TDI AcdObjectInfoG
Acp* - ACPI Pooltags
Adap - Adapter objects
Adbe - Adobe's font driver
Afd? - AFD objects
AfdA -     Afd EA buffer
AfdB -     Afd data buffer
AfdC -     Afd connection structure
AfdD -     Afd debug data
AfdE -     Afd endpoint structure
AfdF -     Afd TransmitFile info
AfdG -     Afd group table
AfdI -     Afd TDI data
AfdL -     Afd local address buffer
AfdP -     Afd poll info
AfdQ -     Afd work queue item
AfdR -     Afd remote address buffer
AfdS -     Afd security info
AfdT -     Afd transport info
AfdW -     Afd work item
AfdX -     Afd context buffer
Afda -     Afd APC buffer (NT 3.51 only)
Afdc -     Afd connect data buffer
Afdd -     Afd disconnect data buffer
Afdf -     Afd TransmitFile debug data
Afdh -     Afd address list change buffer
Afdi -     Afd "set inline mode" buffer
Afdl -     Afd lookaside lists buffer
Afdp -     Afd transport IRP buffer
Afdq -     Afd routing query buffer
Afdr -     Afd ERESOURCE buffer
Afdt -     Afd transport address buffer
Afp  - SFM File server
Aml* - ACPI AMLI Pooltags
ArbA - ARBITER_ALLOCATION_STATE_TAG
ArbL - ARBITER_ORDERING_LIST_TAG
ArbM - ARBITER_MISC_TAG
ArbR - ARBITER_RANGE_LIST_TAG
Arp? - ATM ARP server objects, atmarps.sys
ArpA -     AtmArpS address
ArpB -     AtmArpS buffer space
ArpI -     AtmArpS Interface structure
ArpK -     AtmArpS ARP block
ArpM -     AtmArpS MARS structure
ArpR -     AtmArpS NDIS request
ArpS -     AtmArpS SAP structure
Asy1 - ndis / ASYNC_IOCTX_TAG
Asy2 - ndis / ASYNC_INFO_TAG
Asy3 - ndis / ASYNC_ADAPTER_TAG
Asy4 - ndis / ASYNC_FRAME_TAG
AtC  - IDE disk configuration - atd_conf.c
AtD  - atdisk.c
ATIX - WDM mini drivers for ATI tuner, xbar, etc.
Atk  - Appletalk transport
AtmA - Atoms
AtmT - Atom tables

AuxL - EXIFS Auxlist

Bat? - Battery Class drivers
BatC -     Composite battery driver
BatM -     Control method battery driver
BatS -     Smart battery driver
Batt -     Battery class driver
BIG  - Large session pool allocations (ntos\ex\pool.c)
Bmfd - Font related stuff

Bu*  - burneng.sys from adaptec

Call - kernel callback object signature
call - debugging call tables
CBRe - CallbackRegistration
CcWK - Kernel Cache Manager lookaside list
CcWk - Kernel Cache Manager lookaside list
CM   - Configuration Manager (registry)
CMCa - Configuration Manager Cache (registry)
CMDc - Configuration Manager Cache (registry)
CMkb - registry key control blocks
CMpb - registry post blocks
CMnb - registry notify blocks
CMpe - registry post events
CMpa - registry post apcs
CMsb - registry stash buffer
COMX - serial driver allocations
CPnp - ClassPnP transfer packets - classpnp.sys
BT8x - WDM mini drivers for Brooktree 848,829, etc.

Cc   - Cache Manager allocations (catch-all)
CcBc - Cache Manager Bcb from pool
CcBr - Cache Manager Bitmap range
CcBz - Cache Manager Bcb Zone
CcDw - Cache Manager Deferred Write
CcEv - Cache Manager Event
CcFn - Cache Manager File name for popups
CcOb - Cache Manager Overlap Bcb
CcPc - Cache Manager Private Cache Map
CcSc - Cache Manager Shared Cache Map
CcVa - Cache Manager Initial array of Vacbs
CcVl - Cache Manager Vacb Level structures (large streams)
CcVp - Cache Manager Array of Vacb pointers for a cached stream
CcWq - Cache Manager Work Queue Item
CcZe - Cache Manager Buffer of Zeros

CctX - EXIFX FCB Commit CTX

CdA  - CdAudio filter driver

Cdcc - CDFS Ccb
Cddn - CDFS CdName in dirent
Cdee - CDFS Search expression for enumeration
Cdfd - CDFS Data Fcb
Cdfi - CDFS Index Fcb
Cdfl - CDFS Filelock
CdFn - CDFS Filename buffer
Cdfn - CDFS Nonpaged Fcb
Cdfs - CDFS General Allocation
Cdft - CDFS Fcb Table entry
Cdgs - CDFS Generated short name
Cdic - CDFS Irp Context
Cdil - CDFS Irp Context lite
Cdio - CDFS Io context for async reads
Cdma - CDFS Mcb array
Cdpe - CDFS Prefix Entry
CdPn - CDFS CdName in path entry
Cdpn - CDFS Prefix Entry name
Cdsp - CDFS Buffer for spanning path table
Cdtc - CDFS TOC
Cdun - CDFS Buffer for upcased name
Cdvd - CDFS Buffer for volume descriptor
Cdvp - CDFS Vpb allocated in filesystem

CmcD - HAL CMC Driver Log - hal.dll
CmcK - HAL CMC Kernel Log - hal.dll
CmcT - HAL CMC temporary Log - hal.dll

Cont - Contiguous physical memory allocations for device drivers

CopW - EXIFS CopyOnWrite

CpeD - HAL CPE Driver Log - hal.dll
CpeK - HAL CMC Kernel Log - hal.dll
CpeT - HAL CPE temporary Log - hal.dll

CrtH - EXIFS Create Header

CSdk - Cluster Disk Filter driver
CSnt - Cluster Network driver
CTE  - Common transport environment (ntos\inc\cxport.h, used by tdi)

Cvli - EXIFS Cached Volume Info

D2d  - Device Object to DosName rtns (ntos\rtl\dev2dos.c)
D3Dd - DX D3D driver (embedded in a display driver like s3mvirge.dll)
D851 - 8514a video driver
Dati - ati video driver
DbLo - Debug logging
dcam - WDM mini driver for IEEE 1394 digital camera
Dcl  - cirrus video driver
Ddk  - Default for driver allocated memory (user's of ntddk.h)
Devi - Device objects

Dh 0 - DirectDraw/3D default object
Dh 1 - DirectDraw/3D DirectDraw object
Dh 2 - DirectDraw/3D Surface object
Dh 3 - DirectDraw/3D Direct3D context object
Dh 4 - DirectDraw/3D VideoPort object
Dh 5 - DirectDraw/3D MotionComp object

Dfb  - framebuf video driver
Dfs  - Distributed File System
Dfsm - Eng event allocation (ENG_KEVENTALLOC,ENG_ALLOC) in ntgdi\gre
Dire - Directory objects
Djz  - jzvxl484 video driver
Dlck - deadlock verifier (part of driver verifier) structures
Dmga - mga (matrox) video driver
DmH? - DirectMusic hardware synthesizer
Dmio - Logical Disk Manager driver
Dmld - Logical Disk Manager loader
DmS? - DirectMusic kernel software synthesizer
Dmst - Logical Disk Manager driver static initialization
Dndt - Device node
Dnod - Device node structure
DOPE - Device Object Power Extension (po component)
DPrf - Disk performance filter driver - diskperf.c
Dps5 - NT5 PostScript printer driver
Dpsh - Postcript driver heap memory
Dpsi - psidisp video driver
Dpsm - Postcript driver memory
Dqv  - qv (qvision) video driver
Driv - Driver objects
Drsd - Rasdd Printer Driver Pool Tag.
Dtga - tga video driver
Dump - Bugcheck dump allocations
Dun5 - NT5 Universal printer driver

DV?? - RDR2 DAV MiniRedir Tags
DVCx - AsyncEngineContext, DAV MiniRedir
DVEx - Exchange, DAV MiniRedir
DVFi - FileInfo, DAV MiniRedir
DVFn - FileName, DAV MiniRedir
DVRw - ReadWrite, DAV MiniRedir
DVSc - SrvCall, DAV MiniRedir
DVSh - SharedHeap, DAV MiniRedir

Dvga - vga 16 color video driver
Dvg2 - vga 256 color video driver
Dvg6 - vga 64K color video driver
Dvgr - vga for risc video driver
DW32 - W32 video driver
Dwd  - wd90c24a video driver
Dwp9 - weitekp9 video driver
Dxga - XGA video driver

Efsm - EFS driver
Efsc - EFS driver

Efst = EXIFS FS Statistics

Envr - Environment strings
Err  - Error strings
Evel - EFS file system filter driver lookaside list
Even - Event objects
Evid - Rtl Event ID's
ExWl - Executive worker list entry

Fat  - Fat File System allocations
FatB - Fat allocation bitmaps
FatC - Fat Ccbs
FatD - Fat pool dirents
FatE - Fat EResources
FatF - Fat Fcbs
FatI - Fat IrpContexts
FatL - Fat FAT entry lookup buffer on verify
FatN - Fat Nonpaged Fcbs
FatO - Fat I/O buffer
FatP - Fat output for query retrieval pointers (caller frees)
FatQ - Fat buffered user buffer
FatR - Fat repinned Bcb
FatS - Fat stashed Bpb
FatT - Fat directory allocation bitmaps
FatV - Fat Vcb stat bucket
Fatv - Fat events
FatW - Fat FAT windowing structure
FatX - Fat IO contexts
Fatb - Fat Bcb arrays
Fatd - Fat EA data
Fate - Fat EA set headers
Fatf - Fat deferred flush contexts
Fati - Fat IO run descriptor
Fatn - Fat filename buffer
Fatr - Fat verification-time rootdir snapshot
Fats - Fat verification-time boot sector
Fatv - Fat backpocket Vpb
Fatx - Fat delayed close contexts

Fcbl - EXIFS FCBlock
fboX - EXIFS FOBXVF List

File - File objects
Flop - floppy driver

Flst - EXIFS Freelist

FLex - exclusive file lock
FLfl - exported (non-private) file lock
FLli - per-file lock information
FLln - shared lock tree node
FLsh - shared file lock
FLwl - waiting lock

Fsrc - Filesystem recognizer (fsrec.sys)

Fstb - ntos\fstub

flnk - font link tag used in ntgdi\gre

FS?? - File System Run Time allocations
FSfm - File System Run Time Fast Mutex Lookaside List
FSim - File System Run Time Mcb Initial Mapping Lookaside List
FSrt - File System Run Time allocations (DO NOT USE!)
FSmg - File System Run Time - fsfilter.c
FSrd - File System Run Time - dbcsname.c
FSrm - File System Run Time - largemcb.c
FSrn - File System Run Time - name.c
FSrN - File System Run Time - notify.c
FSro - File System Run Time - oplock.c
FSrs - File System Run Time - stackovf.c
FSun - File System Run Time - unc.c

FTrc - Fault tolerance Slist tag.
FtC  - Fault tolerance driver - config.c
FtM  - Fault tolerance driver - mirror.c
FtS  - Fault tolerance driver - stripe.c
FtT  - Fault tolerance driver - thread.c
FtU  - Fault tolerance driver - ftutil.c
FtV  - Fault tolerance driver - volset.c

G??? - Gdi Objects
G    -     Gdi Generic allocations
Gcac -     Gdi glyph cache
Gcap -     Gdi capture buffer
Gcsl -     Gdi string resource script names
Gdbr -     Gdi driver brush realization
Gdd  -     Gdi ddraw PKEY_VALUE_FULL_INFORMATION
Gdda -     Gdi ddraw attach list
GddD -     Gdi ddraw dummy page
Gdde -     Gdi ddraw event
Gddf -     Gdi ddraw driver heaps
Gddp -     Gdi ddraw driver caps
Gddv -     Gdi ddraw driver video memory list
Gdxd -     Gdi ddraw VPE directdraw object
Gdxs -     Gdi ddraw VPE surface, videoport, capture object
Gdxx -     Gdi ddraw VPE DXAPI object
Gdev -     Gdi GDITAG_DEVMODE
GDev -     Gdi pdev
Gdrs -     Gdi GDITAG_DRVSUP
Gebr -     Gdi ENGBRUSH
gEdg -     Gdi gradient fill triangle
Gffv -     Gdi FONTFILEVIEW
gFil -     Gdi FILEVIEW
GFil -     Gdi engine descriptor list
Gfsb -     Gdi font sustitution list
Gfsm -     Gdi Fast mutex
Ggdv -     Gdi GDITAG_GDEVICE
Gglb -     Gdi temp buffer
Ggls -     Gdi glyphset
Ggb  -     Gdi glyph bits
Ggbl -     Gdi look aside buffers
Ghmg -     Gdi handle manager objects
Gini -     Gdi fast mutex
Gh 0-Gh N  Gdi Handle manager specific object types: defined in w32\ntgdi\inc\ntgdistr.h
Gla0-GlaN  Gdi handle manager specific object types allocated from lookaside memory: defined in w32\ntgdi\inc\ntgdistr.h
Gldv -     Gdi Ldev
Glnk -     Gdi PFELINK
Gmap -     Gdi font map signature table
Gpff -     Gdi physical font file
Gpft -     Gdi font table
Gsem -     Gdi Semaphores
Gsp  -     Gdi sprite
Gspr -     Gdi sprite grow range
Gtmp -     Gdi temporary allocations
Gtmw -     Gdi TMW_INTERNAL
Gxlt -     Gdi Xlate

Gfcb - EXIFS Grow FCB

Hal  - Hardware Abstraction Layer
Hpfs - Pinball (aka Hpfs) allocations
HisC - histogram filter driver - config.c
Hist - histogram filter driver - histgram.c
HidP - HID Parser
HidC - HID Class
HdCl - HID Client Sample Driver

HT01 - GDI Halftone AddCachedDCI() for CurCDCIData
HT02 - GDI Halftone GetCachedDCI() for Threshold
HT03 - GDI Halftone FindCachedSMP() for CurCSMPData
HT04 - GDI Halftone FindCachedSMP() for CurCSMPBmp
HT05 - GDI Halftone HT_CreateDeviceHalftoneInfo() for HT_DHI
HT06 - GDI Halftone pDCIAdjClr() for DEVCLRADJ
HT07 - GDI Halftone ComputeRGB555LUT() for RGBLUT
HT08 - GDI Halftone ColorTriadSrcToDev() for RGB-XYZ
HT09 - GDI Halftone ColorTriadSrcToDev() for CRTX-FD6XYZ Cache
HT10 - GDI Halftone CreateDyesColorMappingTable() for DevPrims
HT11 - GDI Halftone CreateDyesColorMappingTable() for DyeMappingTable
HT12 - GDI Halftone ThresholdsFromYData() for pYData
HT13 - GDI Halftone ComputeHTCellRegress() for pThresholds
HT14 - GDI Halftone CalculateStretch() for InputSI/pSrcMaskLine
HT15 - GDI Halftone CalculateStretch() for PrimColorInfo

IPX  - Nwlnkipx transport
Icp  - I/O completion packets queue on a completion ports
IcpP - NPAGED_LOOKASIDE_LIST I/O completion per processor lookaside pointers
IdeP - atapi IDE
IdeX - PCI IDE
Ifs  - Default file system allocations (user's of ntifs.h)
Info - general system information allocations
Io   - general IO allocations
IoCc - Io completion context
IoEa - Io extended attributes
IoEr - Io error log packets
IoFc - Io name transmogrify operation
IoFs - Io shutdown packet
IoKB - Registry basic key block (temp allocation)
IoNm - Io parsing names
IoRb - Io remote boot related
IoRN - Registry key name (temp allocation)
IoSe - Io security related
IoSh - Io shutdown packet
IoTi - Io timers
IoTt - I/O verifier IRP tracking table
Ioin - Io interrupts
Irp  - Io, IRP packets
Irp+ - I/O verifier allocated IRP packets
IrpB - I/O verifier direct I/O double buffer allocation
Irpd - I/O verifier deferred completion context
Irps - I/O verifier per-IRP session tracking data
Irpt - I/O verifier per-IRP tracking data
IrpC - I/O verifier stack contexts
Irpl - system large IRP lookaside list
Irps - system small IRP lookaside list
Isap - Pnp Isa bus extender
II?? - IP in IP tunneling
IIrf    - Free memory
IIdt    - Data
IITn    - Tunnel
IIhd    - Header
IIpk    - Packet
IIsc    - Send Context
IIts    - Transfer Context
IIwc    - Work Context

Im*  - Imapi.sys from adaptec

IPm? - IP Multicasting
IPmg    - Group
IPms    - Source
IPmo    - Outgoing Interface
IPmm    - Message
IPmf    - Free memory (only in checked builds)

Ip?? - IP Security
IpSI - IPSEC - initial allcoations
IpAT - IPSEC - AH headers in transport mode
IpAU - IPSEC - AH headers in tunnel mode
IpET - IPSEC - ESP headers in transport mode
IpEU - IPSEC - ESP headers in tunnel mode
IpHT - IPSEC - HUGHES headers in transport mode
IpHU - IPSEC - HUGHES headers in tunnel mode
IpAX - IPSEC - Key acquire contexts
IpFI - IPSEC - Filter blocks
IpSA - IPSEC - Security Associations
IpKE - IPSEC - keys
IpTI - IPSEC - timers
IpSQ - IPSEC - stall queues
IpLA - IPSEC - lookaside lists
IpBP - IPSEC - buffer pools
IpSC - IPSEC - send complete context
IpEQ - IPSEC - event queue
IpHW - IPSEC - hardware accleration items
IpCO - IPSEC - IP compression

IrD? - IrDA TDI and RAS drivers

IU?? - IIS Utility Driver
IUDl -     Lookaside list allocations
IUcp -     completion port minipackets

KbdC - Keyboard Class Driver
KCfe - Kernel COM factory entry
Ke   - Kernel data structures
Key  - Key objects
KMIX - Kmixer (wdm audio)
KrbC - Kerberos Client package
KSah - Ks auxiliary stream headers
KSAI -    allocator instance
KSai -    default allocator instance header
KSbi -    event buffered item
KSCI -    clock instance
KSce -    create item entry
KSch -    create handler entry
KSci -    default clock instance header
KScp -    object creation parameters auxiliary copy
KSda -    default allocator
KSdc -    default clock
KSdh -    device header
Ksec -    security device driver
KSed -    event dpc item
KSee -    event entry
KSed -    oneshot event deletion dpc
KSep -    irp system buffer event parameter
KSew -    oneshot event deletion workitem
KSqr -    QM quality report
KSer -    QM error report
KSfd -    filter cache data (MSKSSRV)
KsFI -    filter instance
KSns -    null security object
KSnv -    registry name value
KSoh -    object header
KSop -    object creation parameters
KSpc -    port driver instance FsContext
KSPI -    pin instance
KSpp -    irp system buffer property/method/event parameter
KSpt -    pin type list (MSKSSRV)
KSqf -    query information file buffer
KSsc -    port driver stream FsContext
KSsf -    set information file buffer
KSsh -    stream headers
KSsi -    software bus interface
KSsl -    symbolic link buffer (MSKSSRV)
KSsp -    serialized property set
KsoO -    WDM audio stuff
L2T0 -    ndis\l2tp / MTAG_FREED
L2T1 -    ndis\l2tp / MTAG_ADAPTERCB
L2T2 -    ndis\l2tp / MTAG_TUNNELCB
L2T3 -    ndis\l2tp / MTAG_VCCB
L2T4 -    ndis\l2tp / MTAG_VCTABLE
L2T5 -    ndis\l2tp / MTAG_WORKITEM
L2T6 -    ndis\l2tp / MTAG_TIMERQ
L2T7 -    ndis\l2tp / MTAG_TIMERQITEM
L2T8 -    ndis\l2tp / MTAG_PACKETPOOL
L2T9 -    ndis\l2tp / MTAG_FBUFPOOL
L2Ta -    ndis\l2tp / MTAG_HBUFPOOL
L2Tb -    ndis\l2tp / MTAG_TDIXRDG
L2Tc -    ndis\l2tp / MTAG_TDIXSDG
L2Td -    ndis\l2tp / MTAG_CTRLRECD
L2Te -    ndis\l2tp / MTAG_CTRLSENT
L2Tf -    ndis\l2tp / MTAG_PAYLRECD
L2Tg -    ndis\l2tp / MTAG_PAYLSENT
L2Th -    ndis\l2tp / MTAG_INCALL
L2Ti -    ndis\l2tp / MTAG_UTIL
L2Tj -    ndis\l2tp / MTAG_ROUTEQUERY
L2Tk -    ndis\l2tp / MTAG_ROUTESET
L2Tl -    ndis\l2tp / MTAG_L2TPPARAMS
L2Tm -    ndis\l2tp / MTAG_TUNNELWORK
L2Tn -    ndis\l2tp / MTAG_TDIXROUTE
LANE - LAN Emulation Client for ATM - atmlane.sys
LB?? - LM Datagram receiver allocations
LBan -     Server announcement
LBvb -     View buffer
LBma -     Master announce context
LBxp -     Transport
LBxn -     TransportName
LBxm -     Master name
LBtn -     Transport name
LBea -     Ea buffer
LBdi -     POOL_DOMAIN_INFO
LBds -     Send datagram context
LBci -     Connection info
LBmh -     Mailslot header
LBbl -     Backup List
LBsl -     Browser server list
LBbs -     Browser server
LBgb -     GetBackupList request
LBbr -     GetBackupList response
LBmb -     Mailslot Buffer
LBid -     Illegal datagram context
LBbn -     Name
LBnn -     Name name
LBic -     IRP context
LBwi -     Work item
LBel -     Election context
LBbb -     Become backup context
LBbr -     Become backup request
LBpn -     Paged Name
LBpt -     Paged transport
LBse -     Browser security

Lbuf - EXIFS Large Buffer

LeoC -     Symantec/Norton AntiVirus filter driver
LCam - WDM mini video capture driver for Logitech camera
Lfs  - Lfs allocations
LpcZ - LPC Zone
Lr?? - LM redirector allocations
Lr   -     Generic allocations
Lrcx -     Context blocks of various types
Lrcl -     ConnectListEntries
Lrsl -     ServerListEntries
Lrse -     Security entry
Lrsc -     Search Control Blocks
Lrea -     EA related allocations
Lric -     Instance Control Blocks
Lrfc -     File Control Blocks
Lrfl -     Fcb Locks
Lrfp -     Fcb Paging locks
Lrcn -     Computer Name
Lrdn -     Domain Name
Lr?? -     Buffers used for FsControlFile APIs
Lrlc -     Lock Control Blocks
Lrlb -     Lock Control Block buffers
Lrnf -     Non paged FCB
Lrnt -     Non paged transport
Lrps -     Paged security entry
Lrte -     Transport event.
Lrxx -     Transceive context blocks
Lr!! -     Cancel request context blocks
Lrmt -     MPX table
Lrme -     MPX table entries
Lrsx -     Send contexts
Lraw -     Async write context
Lrwb -     Write behind buffer header
Lrbb -     Write behind buffer
Lrwq -     Work queue item
Lrac -     ACL for redirector
Lrds -     Security Descriptor for redirector
Lrsm -     SMB buffer
Lrds -     Duplicated ansi string
Lrdu -     Duplicated unicode string
Lxpt -     Transport
Lrtc -     Transport connection
Lrna -     Netbios Addresses
Lrca -     Temporary storage used in name canonicalization
Lr2x -     Transact SMB context
Lrpt -     Primary transport server list
Lrso -     Operating system name
Lref -     Reference history (debug only)
LS?? - LM server allocations
LSac -     BlockTypeAdminCheck
LSas -     BlockTypeAdapterStatus
LSbf -     buffer descriptor
LScd -     comm device
LScn -     connection
LSdb -     data buffer
LSdc -     BlockTypeDirCache
LSdi -     BlockTypeDirectoryInfo
LSep -     endpoint
LSfn -     BlockTypeFSName
LSlf -     LFCB
LSlr -     BlockTypeLargeReadX
LSmf -     MFCB
LSmi -     BlockTypeMisc
LSnh -     nonpaged block header
LSni -     BlockTypeNameInfo
LSop -     oplock break wait
LSpc -     paged connection
LSpm -     paged MFCB
LSpr -     paged RFCB
LSrf -     RFCB
LSsc -     search(core)
LSsd -     BlockTypeShareSecurityDescriptor
LSsf -     BlockTypeDfs
LSsh -     share
LSsp -     search(core complete)
LSsr -     search
LSss -     session
LStb -     table
LStc -     tree connect
LSti -     timer
LStr -     transaction
LSvi -     BlockTypeVolumeInformation
LSwi -     initial work context
LSwn -     normal work context
LSwq -     BlockTypeWorkQueue
LSwr -     raw work context
LSws -     BlockTypeWorkContextSpecial
LSnd - WDM mini sound driver for Logitech video camera
LXMK - kernel mixer line driver (KMXL - looks like they got their tag backwards)

MXF  - DirectMusic (MIDI Transform Filter)

MapP - PNP map
Mapr - arc firmware registry routines

McaC - HAL MCA corrected Log - hal.dll
McaD - HAL MCA Driver Log - hal.dll
McaK - HAL MCA Kernel Log - hal.dll
McaP - HAL MCA Log from previous boot - hal.dll
McaT - HAL MCA temporary Log - hal.dll

MCAM - WDM mini driver for Intel USB camera
MCDx - OpenGL MCD server (mcdsrv32.dll) allocations
MCDd - OpenGL MCD driver (embedded in a display driver like s3mvirge.dll)
Mdl  - Io, Mdls
MdlP - MDL per processor lookaside list pointers
MePr - In-memory print buffer
Mm   - general Mm Allocations
MmBk - Mm banked sections
MmCa - Mm control areas for mapped files
MmCd - Mm fork clone descriptors
MmCh - Mm fork clone headers
MmCi - Mm control areas for images
MmCm - Calls made to MmAllocateContiguousMemory
MmDb - NtMapViewOfSection service
MmDT - Mm debug
MmEx - Mm events
MmHi - Mm image entry - allocated per session
MmHn - Mm sessionwide address name string entry
MmHv - Mm sessionwide address entry
MmIn - Mm inpaged io structures
MmLd - Mm load module database
MmPg - Mm page table pages at init time
MmRw - Mm read write virtual memory buffers
MmSb - Mm subsections
MmSe - Mm secured VAD allocation
MmSg - Mm segments
MmSt - Mm section object prototype ptes
Mmdl - Mm Mdls for flushes
Mmpp - Mm prototype PTEs for pool
MmVd - Mm virtual address descriptors for mapped views
MmVs - Mm virtual address descriptors short form (private views)
Mmxx - Mm temporary allocations

Msdv - WDM mini driver for IEEE 1394 DV Camera and VCR
MST? - MSTEE (mstee.sys)
MSTa -    associated stream header
MSTc -    filer connection
MSTd -    data format
MSTf -    filter instance
MSTp -    pin instance
MSTs -    stream header

MuoC - MOUSE_POOL_TAG Mouse Class Driver

MsFc - Mailslot CCB, Client control block. Each client with an opened mailslot has one of these
MsFC - Mailslot root CCB, A client control block for the top level mailslot directory
MsFd - Mailslot data entry write buffer, This is writes buffered inside mailslots
MsFD - Mailslot root DCB and its name buffer
MsFf - Mailslot FCB, File control block, Service side block for each created mailslot.
MsFg - Mailslot global resource
MsFn - Mailslot temporary name buffer
MsFN - Mailslot FCB name buffer, name for each created mailslot
MsFr - Mailslot read buffer, buffer created for pended reads issued.
MsFt - Mailslot query template, used for directory queries.
MsFw - Mailslot work context block, blocks create when we need to timeout reads.

MsvC - Msv/Ntlm client package
Mup  - Multiple UNC provider allocations
Muta - Mutant objects
NBF  - general NBF allocations
NBFa - NBF address object
NBFb - NBF receive buffer
NBFc - NBF connection object
NBFd - NBF packet pool descriptor
NBFe - NBF bind & export names
NBFf - NBF address file object
NBFg - NBF registry path name
NBFi - NBF tdi connection info
NBFk - NBF loopback buffer
NBFl - NBF link object
NBFn - NBF netbios name
NBFo - NBF config data
NBFp - NBF packet
NBFq - NBF query buffer
NBFr - NBF request
NBFs - NBF provider stats
NBFt - NBF connection table
NBFu - NBF UI frame
NBFw - NBF work item
NBI  - NwlnkNb transport
NBS  - general NetBIOS allocations
NBSa -     address block
NBSc -     connection block
NBSe -     EA buffer
NBSf -     FCB
NBSl -     LANA block
NBSn -     copy of user NCB
NBSr -     registry allocations
NBSx -     XNS NETONE address (connect block)
NBSy -     NetBIOS address (connect block)
NBSz -     NetBIOS address (listen block)

NCSt - EXIFS NC

// NDIS stuff
ND   - general NDIS allocations
NDDl - NDIS_TAG_DBG_LOG
NDMb - NDIS_TAG_MAC_BLOCK
NDPX -     NDIS Proxy allocations
NDPa - Apple Talk
NDPb - NBF
NDPi - NWLNKIPX
NDPn - NWLNKNB
NDPp - Packet Scheduler.
NDPs - NWLNKSPX
NDPt - TCPIP
NDPw - WAN_PACKET_TAG
NDam -     NdisAllocateMemory
NDan -     adapter name
NDco - NDIS_TAG_CO
NDd  - NDIS_TAG_DBG
NDdb -     DMA block
NDdl - NDIS_TAG_DBG_L
NDdp - NDIS_TAG_DBG_P
NDds - NDIS_TAG_DBG_S
NDdt - NDIS_TAG_DFRD_TMR
NDfa - NDIS_TAG_FILTER_ADDR
NDlb -     lookahead buffer
NDlp - NDIS_TAG_LOOP_PKT
NDmb -     MAC block
NDmr -     map register entry array
NDoa - NDIS_TAG_OID_ARRAY
NDob -     open block
NDpb -     protocol block
NDpf - NDIS_TAG_FILTER
NDpk - NDIS_TAG_PKT_PATTERN
NDpp -     packet pool
NDrl -     resource list
NDrp - NDIS_TAG_REGISTRY_PATH
NDrq - NDIS_TAG_Q_REQ
NDsi -     EISA slot information
NDsm -     Cached shared memory descriptor
NDst - NDIS_TAG_STRING
NDw0 - NDIS_TAG_WMI_REG_INFO
NDw1 - NDIS_TAG_WMI_GUID_TO_OID
NDw2 - NDIS_TAG_WMI_OID_SUPPORTED_LIST
NDw3 - NDIS_TAG_WMI_EVENT_ITEM
NDwi - NDIS_TAG_WORK_ITEM

Nb?? -  NetBT allocations
Nls  - Nls strings
Nmdd - NetMeeting display driver miniport 1 MB block
None - call to ExAllocatePool

Npf* - Npfs Allocations
NpFc - CCB, client control block
NpFC - ROOT_DCB CCB
NpFD - DCB, directory block
NpFg - Global storage
NpFi - NPFS client info buffer.
NpFn - Name block
NpFq - Query template buffer used for directory query
NpFr - DATA_ENTRY records (read/write buffers)
NpFs - Client security context
NpFw - Write block
NpFW - Write block


NS?? - Netware server allocations

Ntf? - NTFS Specific allocation tags
Ntf0 -     general pool allocation
Ntf9 -     Large Temporary Buffer
NtfC -     CCB
Ntfc -     CCB_DATA
NtfD -     DEALLOCATED_RECORDS
NtfE -     INDEX_CONTEXT
NtfF -     FCB_INDEX
Ntff -     FCB_DATA
NtfI -     NTFS_IO_CONTEXT
Ntfi -     IRP_CONTEXT
NtfK -     KEVENT
Ntfk -     FILE_LOCK
Ntfl -     LCB
NtfM -     NTFS_MCB_ENTRY
Ntfm -     NTFS_MCB_ARRAY
NtfN -     NUKEM
Ntfn -     SCB_NONPAGED
Ntfo -     SCB_INDEX normalized named buffer
NtfQ -     QUOTA_CONTROL_BLOCK
Ntfq -     General Allocation with Quota
NtfR -     READ_AHEAD_THREAD
Ntfr -     ERESOURCE
NtfS -     SCB_INDEX
Ntfs -     SCB_DATA
NtfT -     SCB_SNAPSHOT
Ntft -     SCB (Prerestart)
NtfV -     VPB
Ntfv -     COMPRESSION_SYNC
Ntfw -     Workspace
Ntfx -     General Allocation
NtF? - NTFS tags based on source module
NtFa -     AllocSup.c
NtFA -     AttrSup.c
NtFB -     BitmpSup.c
NtFC -     Create.c
NtFD -     DevioSup.c
NtFd -     DirCtrl.c
NtFE -     Ea.c
NtFF -     FileInfo.c
NtFf -     FsCtrl.c
NtFI -     IndexSup.c
NtFL -     LogSup.c
NtFM -     McbSup.c
NtFN -     NtfsData.c
NtFO -     ObjIdSup.c
NtFQ -     QuotaSup.c
NtFR -     RestrSup.c
NtFS -     SecurSup.c
NtFs -     StrucSup.c
NtFU -     usnsup.c
NtFV -     VerfySup.c
NtFv -     ViewSup.c
NtFW -     Write.c

Nwcs - Client Services for NetWare
NwFw - ntos\tdi\fwd


ObSq - object security descriptors (query)
ObCi - captured information for ObCreateObject
ObCI - object creation lookaside list
ObDi - object directory
ObHd - object handle count data base
ObNm - object names
ObNM - name buffer per processor lookaside pointers
ObZn - object zone
ObjT - object type objects
Obtb - object tables via EX handle.c
ObTR - object table ERESOURCEs
Obeb - object tables extra bit tables via EX handle.c
ObDm - object device map
ObSc - Object security descriptor cache block
ObSt - Object Manager temporary storage

OHCI - Open Host Controller Interface for USB
ohci - 1394 OHCI host controller driver

Ovfl - The internal pool tag table has overflowed - usually this is a result of nontagged allocations being made

OvfL - EXIFS FCBOVF List

PaeD - PAE top level directory allocation blocks

ParC - Parallel class driver
ParL - Parallel link driver
ParP - Parallel port driver
ParV - ParVdm driver for vdm<->parallel port communciation

PciB - PnP pci bus enumerator - pci.sys

PcCi - WDM audio port class adapter device object stuff
PcCr - WDM audio stuff
PcDi - WDM audio stuff
PcDm - DirectMusic MXF objects (WDM audio)
PcFM - WDM audio FM synthesizer
PcFp - WDM audio stuff
PcIc - WDM audio stuff
PcIl - WDM audio stuff
PcNw - WDM audio stuff
PcPc - WDM audio stuff
PcPr - WDM audio stuff
PcSX - WDM audio stuff
PcSl - WDM audio stuff
PcSt - WDM audio stuff
PcSx - WDM audio stuff
PcUs - WDM audio stuff

Pcmc - Pcmcia bus enumerator, general structures - pcmcia.sys
Pcic - Pcmcia bus enumerator, PCIC/Cardbus controller specific structures
Pcdb - Pcmcia bus enumerator, Databook controller specific structures

Pgm? - Pgm (Pragmatic General Multicast) protocol: RMCast.sys

Plcp - Cache aware pushlock list (array of puchlock addresses)
Plcl - Cache aware pushlock entry. One per processor
PNCH - Power Notify Channel
PNCL - Power Notify channel list
PNDP - Power Abort Dpc Routine
PNI  - Power Notify Instance

Pool - Pool tables, etc.
PooL - Phase 0 initialization of the executive component, paged and nonpaged small pool lookaside structures
Port - Port objects

PoSL - Power shutdown event list

Prof - Profile objects

POWI - Power Work Item (executive worker thread work item entry)
PSwt - Power switch structure
PSTA - Po registered system state
PDss - Po device system state

Prcr - Processr hal driver allocations
Proc - Process objects
Ps   - general ps allocations
PsAp - Process APC queued by user mode process
PsCa - APC queued at thread create time.
PsCr - Working set change record, should be a temporary allocation
PsEx - Process exit APC
PsIm - Thread impersonation (PS_IMPERSONATE_INFORMATION)
Psjb - Job set array, should be a temporary allocation
PsLd - Process LDT information blocks
PsQb - Process quota block
PsTf - Job object token filter
Pstb - Process tables via EX handle.c
PsTp - Thread termination port block
PsWs - Process working set watch array


PSE3 - Physical Size Extension driver - pse36.sys

// pnp/io stuff
PpEB - PNP_POOL_EVENT_BUFFER
PpEE - PNP_DEVICE_EVENT_ENTRY_TAG
PpEL - PNP_DEVICE_EVENT_LIST_TAG
PpUB - PNP_USER_BLOCK_TAG
PpWI - PNP_DEVICE_WORK_ITEM_TAG
Ppdd - new Plug-And-Play driver entries and IRPs
Ppde - routines to perform device removal
Ppei - Eisa related code
Ppen - routines to perform device enumeration
Ppin - plug-and-play initialization
Ppio - plug-and-play IO system APIs
Ppre - resource allocation and translation
Pprl - routines to manipulate relations list
Ppsu - plug-and-play subroutines for the I/O system

// point-to-point tunneling protocol stuff
PPTP - PPTP_MEMORYPOOL_TAG
PPT0 - PPTP_TDIADDR_TAG
PPT1 - PPTP_TDICONN_TAG
PPT2 - PPTP_CONNINFO_TAG
PPT3 - PPTP_ADDRINFO_TAG
PPT4 - PPTP_TIMEOUT_TAG
PPT5 - PPTP_TIMER_TAG
PPT6 - PPTP_TDICOTS_TAG
PPT7 - PPTP_WRKQUEUE_TAG
PPT8 - PPTP_SEND_CTRLDATA_TAG
PPT9 - PPTP_SEND_ACKDATA_TAG
PPTa - PPTP_SEND_DGRAMDESC_TAG
PPTb - PPTP_TDICLTS_TAG
PPTc - PPTP_RECV_CTRLDESC_TAG
PPTd - PPTP_RECV_CTRLDATA_TAG
PPTe - PPTP_RECV_DGRAMDESC_TAG
PPTf - PPTP_RECV_DGRAMDATA_TAG
PPTg - PPTP_RECVDESC_TAG
PPTh - PPTP_ENGINE_TAG
PPTi - PPTP_RECVDATA_TAG

PSC? - Packet Scheduler (PSCHED) Tags

PSC0 - NDIS Request
PSC1 - GPC Client Vc
PSC2 - WanLink
PSC3 - Miscellaneous allocations
PSC4 - WMI
PSCa - Adapter
PSCb - CallParameters
PSCc - PipeContext
PSCd - FlowContext
PSCe - ClassMapContext
PSCf - Adapter Profile
PSCg - Component

PX1 - ndis ProviderEventLookaside (pxntinit.c)

p2?? - Permedia2 display driver - perm2dll.dll
p2d3 - Permedia2 display driver - d3d.c
p2d6 - Permedia2 display driver - d3ddx6.c
p2de - Permedia2 display driver - debug.c
p2ds - Permedia2 display driver - d3dstate.c
p2dt - Permedia2 display driver - d3dtxman.c
p2su - Permedia2 display driver - ddsurf.c
p2en - Permedia2 display driver - enable.c
p2fi - Permedia2 display driver - fillpath.c
p2he - Permedia2 display driver - heap.c
p2hw - Permedia2 display driver - hwinit.c
p2cx - Permedia2 display driver - p2ctxt.c
p2pa - Permedia2 display driver - palette.c
p2pe - Permedia2 display driver - permedia.c
p2tx - Permedia2 display driver - textout.c

P3D? - Permedia3 display driver - DirectDraw/3D
P3G? - Permedia3 display driver - GDI

Qnam - EXIFS Query Name

Qp?? - Generic Packet Classifier (MSGPC)
Qppn -      Queued Notifications
Qppi -      Pending Irp structures
Qpci -      CfInfo
Qpct -      Client blocks
Qppa -      Pattern blocks
Qphf -      HandleFactory
Qpph -      PathHash
Qprz -      Rhizome
Qppd -      GenPatternDb
Qpfd -      FragmentDb
Qpcf -      ClassificationFamily
Qpcd -      CfInfoData
Qpcb -      ClassificationBlock
Qppt -      Protocol
Qpdg -      Debug

RB?? - RedBook Filter Driver, static allocations
RBEv - RedBook - Thread Events
RBRl - RedBook - Remove lock
RBRg - RedBook - driverExtension->RegistryPath
RBSe - RedBook - Serialization tracking for checked builds
RBWa - RedBook - Wait block for system thread

rb?? - RedBook Filter Driver, dynamic allocations
rbBu - RedBook - Buffer for read/stream
rbIr - RedBook - Irp for read/stream
rbIp - RedBook - Irp pointer block
rbMd - RedBook - Mdl for read/stream
rbMp - RedBook - Mdl pointer block
rbRc - RedBook - Read completion context
rbRx - RedBook - Read Xtra info
rbSc - RedBook - Stream completion context
rbSx - RedBook - Stream Xtra info
rbTo - RedBook - Cached table of contents

ReEv - Resource Event
ReSe - Resource Semaphore
ReTa - Resource Extended Table

RRle - RTL_RANGE_LIST_ENTRY_TAG
RRlm - RTL_RANGE_LIST_MISC_TAG

RS?? - Remote Storage
RSFS -      Recall Queue
RSFN -      File Name
RSSE -      Security info
RSWQ -      Work Queue
RSQI -      Queue info
RSLT -      Long term data
RSIO -      Ioctl Queue
RSFO -      File Obj queue
RSVO -      Validate Queue
RSER -      Error log data
RWan - Raw WAN driver - rawwan.sys

Rx?? - Rdr2 rdbss.sys stuff
RxSc -         SrvCall
RxNr -         NetRoot
RxVn -         VNetRoot
RxFc -         FCB
RxSo -         SrvOpen
RxFx -         Fobx
RxNf -         nonpaged FCB
RxWq -         work queue
RxBm -         buffering manager
RxMs -         misc.
RxIr -         RxContext (IrpContext)
RxMx -         minirdr dispatch table
RxNc -         NameCache (not currently used)

RxCt -         connection engine transport
RxCa -         connection engine address
RxCc -         connection engine connection
RxCv -         connection engine VC
RxCd -         connection engine TDI

RxSp -         srvcall calldown params (special build only)
RxVp -         vnetroot calldown params (special build only)
RxTm -         timer context (special build only)
RxDc -         querydirectory pattern (special build only)
RxCx -         connection engine misc. (special build only)

S3   - S3 video driver
SBad - bad block simulator - simbad.c

sbp2 - Sbp2 1394 storage port driver

SC?? - Smart card driver tags
SCLb -  Smart card driver library
SCB8 -  Bull CP8 Transac serial reader
SCB3 -  Bull SmarTlp PnP
SCS4 -  SCM Microsystems pcmcia reader
SCl0 -  Litronic 220

Sc?? - Mass storage driver tags

ScB<number> - ClassPnP misc allocations - classpnp.sys
ScC<number> - ClassPnP misc allocations - classpnp.sys

ScC? -   CdRom
ScCE -      device control synch event
ScCG -      disk geometry buffer
ScCH -      hitachi error buffer
ScCI -      sense info buffers
ScCS -      srb allocation
ScCM -      mode data buffer
ScCP -      read capacity buffer
ScCp -      play active checks
ScCQ -      read sub q buffer
ScCR -      raw mode read buffer
ScCT -      read toc buffer
ScCt -      toshiba error buffer
ScCU -      update capacity path
ScCV -      volume control buffer
ScCv -      volume control buffer

ScD? -   Disk
ScD  -      generic tag
ScDa -      SMART
ScDA -      Info Exceptions
ScDC -      disable cache paths
ScDb -      ClassPnP debug globals buffer - classpnp.sys
ScDc -      disk allocated completion c
ScDG -      disk geometry buffer
ScDg -      update disk geometry paths
ScDI -      sense info buffers
ScDp -      pnp ids
ScDM -      mode data buffer
ScDM -      mbr checksum code
ScDN -      disk name code
ScDP -      read capacity buffer
ScDp -      disk partition lists
ScDS -      srb allocation
ScDs -      start device paths
ScDU -      update capacity path
ScDW -      work-item context
ScMC -      medium changer allocations

ScF? -   FtDisk
ScFt -      FtDisk allocations

ScIO - ClassPnP device control - classpnp.sys

ScL? -   Classpnp
ScLA -      allocation to check for autorun disable
ScLF -      File Object Extension
ScLc -      Cache filters
ScLf -      Fault prediction
ScLm -      Mount
ScLM -      Media Change Detection
ScLq -      Release queue
ScLw -      WMI
ScLW -      Power

ScNo - ClassPnP notification - classpnp.sys

ScP? -   Scsiport
ScPa -      Hold registry data
ScPA -      Access Ranges
ScPb -      Get Bus Dat Holder
ScPB -      Queuetag BitMap
ScPc -      Fake common buffer
ScPC -      reset bus code
ScPd -      Pnp id strings
ScPD -      SRB_DATA allocations
ScPE -      Scatter gather lists
ScPG -      Global memory
ScPh -      HwDevice Ext
ScPi -      Sense Info
ScPI -      Init data chain
ScPl -      remove lock tracking
ScPL -      scatter gather lists
ScPm -      address mapping lists
ScPM -      scatter gather lists
ScPp -      device & adapter enable
ScpP -      scsi PortConfig copies
ScPq -      inquiry data
ScPQ -      request sense
ScPr -      resource list copy
ScPS -      registry allocations
ScPt -      legacy request rerouting
ScPT -      interface mapping
ScPu -      device relation structs
ScPv -      KEVENT
ScPV -      Device map allocations
ScPw -      Wmi Events
ScPW -      Wmi Requests
ScPx -      Report Luns
ScPY -      Report Targets
ScPZ -      Device name buffer

ScR? -   Partition Manager
ScRv -      Dependant volume relations lists

ScsC - non-pnp SCSI CdRom
ScsD - non-pnp SCSI Disk
ScsH - non-pnp SCSI from class.h (class2)
ScsI - non-pnp SCSI port internal
ScsL - non-pnp SCSI class.c driver allocations
ScsP - non-pnp SCSI port.c
Scs$ - Tag for pnp class driver's SRB lookaside list

ScUn - Default Tag for pnp class driver allocations

ScV? -  Dvd functionality in cdrom.sys
ScVk -      read buffer for DVD keys
ScVK -      write buffer for DVD keys
ScVS -      buffer for reads of DVD on-disk structures

SdCc - ObsSecurityDescriptorCache / SECURITY_DESCRIPTOR_CACHE_ENTRIES

Sdba - Application compatibility Sdb* allocations

Se   - General security allocations
SeAc - Security ACL
SeAi - Security Audit Work Item
SeAp - Security Audit Parameter Record
SeCL - Security CONTEXT_TAG
SeFS - SEP_FILE_SYSTEM_NOTIFY_CONTEXT
SeLs - Security Logon Session
SeLu - Security LUID and Attributes array
SeLS - Security Logon Session tracking array
SeLw - Security LSA Work Item
SeOn - Security Captured Object Name information
SePr - Security Privilege Set
SeSa - Security SID and Attributes
SeSc - Captured Security Descriptor
SeSd - Security Descriptor
SeSi - Security SID
SeTa - Security Temporary Array
SeTd - Security Token dynamic part
SeTn - Security Captured Type Name information
SeUs - Security Captured Unicode string

Sect - Section objects
Sema - Semaphore objects
Senm - Serenum (RS-232 serial bus enumerator)
SimB - Simbad (bad sector simulation driver) allocations
SIfs - Default tag for user's of ntsrv.h
sidg - GDI spooler events
Sis  - Single Instance Store (dd\sis\filter)
SisB -         SIS per file object break event
SisC -         SIS common store file object
SisF -         SIS per file object
SisL -         SIS per link object
SisS -         SIS SCB
Setp - SETUPDD SpMemAlloc calls

SW?? - Software Bus Enumerator
SWbi -         bus ID
SWbr -         bus reference
SWda -         POOLTAG_DEVICE_ASSOCIATION
SWdn -         device name
SWdr -         device reference
SWdr -         POOLTAG_DEVICE_DRIVER_REGISTRY
SWfd -         POOLTAG_DEVICE_FDOEXTENSION
SWid -         device ID
SWii -         instance ID
SWip -         POOLTAG_DEVICE_INTERFACEPATH
SWki -         key information
SWpd -         POOLTAG_DEVICE_PDOEXTENSION
SWre -         relations
SWrp -         reparse string
SWrs -         reference string

Sm?? - rdr2 smb mini
SmCe -         smbmini connection engine
SmMm -         smbmini managed datastructures
SmAd -         session setup/admin exchange
SmRw -         read/write path
SmTr -         transact exchange
SmMs -         misc.
SmTp -         server transport tag
SmRb -         remote boot

SmFc -         fsctl structures  (special build only)
SmDc -         dir query buffer (special build only)
SmPi -         pipeinfo buffer (special build only)
SmDO -         deferred open context  (special build only)
SmQP -         params for directory query transact  (special build only)
SmRx -         minirdr generated RxContext  (special build only)

SmVr -         VNetroot  (special build only)
SmSr -         Server  (special build only)
SmSe -         Session  (special build only)
SmNr -         NetRoot  (special build only)
SmMa -         mid atlas  (special build only)
SmMt -         mailslot buffer  (special build only)
SmVc -         smbmini connectionengine VC  (special build only)
SmEc -         echo buffer  (special build only)
SmKs -         Kerberos blob  (special build only)

SPX  - Nwlnkspx transport
SQOS - Security quality of service in IO

Sr?? - System Restore file system filter driver - sr.sys
SrCo -         SR's control object
SrSC -         Stream contexts
SrDB -         Debug information for lookup blob
SrDL -         Device list
SrFE -         File information buffer
SrFN -         File name
SrHB -         Hash bucket
SrHH -         Hash header
SrHK -         Hash key
SrLB -         Log buffer
SrLC -         Logging context
SrLE -         Log entry
SrLT -         Lookup blob
SrMP -         Mount point information
SrOI -         Overwrite information
SrPC -         Persistant configuration information
SrRB -         Rename buffer
SrRG -         Logger context
SrRH -         Reparse data size
SrRR -         Registry information
SrSD -         Security data information
SrST -         Stream data information
SrTI -         Directory delete information
SrWI -         Work queue item

ST*  - New MMC compliant storage drivers - NotDefined.sys - henrygab

Stac - Stack Trace Database - i386 checked and built with NTNOFPO=1 only
Strg - Dynamic Translated strings
Strm - Streams and streams transports allocations
SwMi - SWMidi KS filter (WDM Audio)
Symb - Symbolic link objects
Symt - Symbolic link target strings

SYSA - Sysaudio (wdm audio)

TAPI - ntos\ndis\ndistapi

TC?? - TCP/IP network protocol
TCh? -     TCP/IP header pools
TCPC -     TCP connection pool
TCPT -     TCB pool
TCPY -     SYN-TCB pool
TCPr -     TCP request pool

TDIc - TDI resource
TDId - TDI resource
TDIe - TDI resource
TDIf - TDI resource
TDIg - TDI resource
TDIk - TDI resource
TDIu - TDI resource
TDIv - TDI resource

thdd - DirectDraw/3D handle manager table

Thre - Thread objects
Time - Timer objects
Toke - Token objects

Tran - EXIFS Translate

TSBV - WDM mini driver for Toshiba 750 capture
TSdd - RDPDD - Hydra Display Driver
TSmc - PDMCS - Hydra MCS Protocol Driver
TSwd - RDPWD - Hydra Winstation Driver

TTFC - Font file cache
Ttfd - TrueType Font driver

Thrm - Thermal zone structure
TunL - Tunnel cache lookaside-allocated elements
TunP - Tunnel cache oddsized pool-allocated elements
TunK - Tunnel cache temporary key value

Type - Type objects

UCAM - USB digital camera library
UDFV - Udfs volume descriptor sequence descriptor buffer

Udf1 - Udfs file set descriptor buffer
Udf2 - Udfs volmume recognition sequence descriptor buffer
Udf2 - Udfs volume descriptor sequence descriptor buffer
UdfC - Udfs CRC table
UdfD - Udfs FID buffer for view spanning
UdfF - Udfs nonpaged Fcb errata
UdfI - Udfs IO context
UdfL - Udfs IRP context lite (delayed close)
UdfS - Udfs short file name
UdfT - Udfs generic table entry
Udfb - Udfs IO buffer
Udfc - Udfs IRP context
Udfd - Udfs file Fcb
Udfe - Udfs enumeration match expression
Udff - Udfs normalized full filename
Udfi - Udfs directory Fcb
Udfl - Udfs Lcb
Udfp - Udfs Pcb
Udfs - Udfs Sparing Mcb
Udft - Udfs CDROM TOC
Udfv - Udfs Vpb
Udfx - Udfs Ccb

Udp  - Udp protocol (TCP/IP driver)
Ufsc - User FULLSCREEN
UHCD - Universal Host Controller (USB - Intel Controller)
UHUB - Universal Serial Bus Hub

Ul?? - http.sys tags. Note: In-use tags are of the form "Ul??".
       Free tags are of the form "xl??"; i.e., the leading 'U'
       has been replaced with an 'x'.
UlAB - http.sys Auxiliary Buffer
UlAO - http.sys App Pool Object
UlAP - http.sys App Pool Process
UlAR - http.sys App Pool Resource
UlCC - http.sys Control Channel
UlCE - http.sys Config Group Tree Entry
UlCH - http.sys Config Group Tree Header
UlCI - http.sys Config Group URL Info
UlCJ - http.sys Config Group Object Pool
UlCK - http.sys Chunk Tracker
UlCL - http.sys Config Group LogDir
UlCO - http.sys Connection Pool
UlCq - http.sys Config Group Resource
UlCR - http.sys Cache Tracker
UlCT - http.sys Config Group Timestamp
UlCY - http.sys Connection Count Entry
UlDB - http.sys Debug
UlDH - http.sys Date Header Resource
UlDO - http.sys Disconnect Object
UlDq - http.sys Disconnect Resource
UlDR - http.sys Deferred Remove Item
UlDT - http.sys Debug Thread Pool
UlEP - http.sys Endpoint
UlFC - http.sys File Cache Entry
UlFP - http.sys Filter Process
UlFT - http.sys Filter Channel
UlHC - http.sys Http Connection
UlHq - http.sys Http Connection Resource
UlHR - http.sys Internal Request
UlHV - http.sys Header Value
UlIC - http.sys Irp Context
UlID - http.sys Conn ID Table
UlIR - http.sys Internal Response
UlLA - http.sys Large Alloc
UlLB - http.sys Log Data Buffer
UlLD - http.sys Log Field
UlLF - http.sys Log File Entry
UlLG - http.sys Log Generic
UlLL - http.sys Log File Buffer
UlLR - http.sys Log List Resourc3
UlNP - http.sys Non-Paged Data
UlNO - http.sys NSGO Pool
UlOT - http.sys Opaque ID Table
UlPB - http.sys APool Proc Binding
UlPL - http.sys Pipeline
UlQF - http.sys TCI Filter
UlQG - http.sys TCI Generic
UlQI - http.sys TCI Interface
UlQL - http.sys TCI Flow
UlQR - http.sys TCI Resource
UlQT - http.sys TCI Tracker
UlQW - http.sys TCI WMI
UlRB - http.sys Receive Buffer
UlRD - http.sys Registry Data
UlRE - http.sys Request Body Buffer
UlRP - http.sys Request Buffer
UlRS - http.sys Non-Paged Resource
UlSS - http.sys Simple Status
UlTR - http.sys Tracker
UlSC - http.sys SSL Cert Data
UlSD - http.sys Security Data
UlSO - http.sys Site Counter Entry
UlTA - http.sys Address Pool
UlTT - http.sys Thread Tracker
UlUC - http.sys Uri Cache Entry
UlUH - http.sys HTTP Unknown Header
UlUL - http.sys URL
UlUM - http.sys URL Map
UlVH - http.sys Virtual Host
UlWC - http.sys Work Context
UlWI - http.sys Work Item
UlZR - http.sys Zombie Resource

UndP - EXIFS Underlying Path

Usac - ACCEL
Usai - ATTACHINFO
Usal - ALTTAB
USBD - Universal Serial Bus Class Driver
Uscb - CLIPBOARD
Uscc - CALLBACK
Usch - SCHEME
Usci - CLIENTTHREADINFO
Uscl - CLASS
Uscm - SCANCODEMAP
Uscp - CLIPBOARDPALETTE
Uscr - COLORS
Usct - CHECKPT
Uscu - CURSOR
Uscv - COLORVALUES
Usd1 - DDE1
Usd2 - DDE2
Usd3 - DDE3
Usd4 - DDE4
Usd5 - DDE5
Usd6 - DDE6
Usd7 - DDE7
Usd8 - DDE8
Usd9 - DDE9
UsdA - DDEa
UsdB - DDEb
UsdC - DDEc
UsdD - DDEd
UsdE - DDE
Usdc - DCE
Usdi - DISPLAYINFO
Usdm - DEVMODE
Usds - DRAGDROP
Usdv - DEVICECHANGE
Usev - EVENT
Usfs - FULLSCREEN
Usgg - GDI
Usgh - GRANTEDHANDLES
Ushk - HOTKEY
Ushl - HUNGLIST
Usih - IMEHOTKEY
Usim - IME
Usjb - W32JOB
Usjx - W32JOBEXTRA
Uskb - KBDLAYOUT
Uskf - KBDFILE
Uskr - KBDTRANS
Usks - KBDSTATE
Uskt - KBDTABLE
Usla - LOOKASIDE
Usld - LOGDESKTOP
Uslr - LOCKRECORD
Usmi - MIRROR
Usmr - MONITORRECTS
Usms - MOVESIZE
Usmt - MENUSTATE
Usno - NONE - don't use
Usny - NOTIFY
Uspi - PROCESSINFO
Uspm - POPUPMENU
Uspn - PROFILEUSERNAME
Uspp - PNP
Uspr - PROFILE
Usqm - QMSG
Usqu - Q
Usrt - RTL
Ussa - SMS_ASYNC
Ussb - SPB
Ussc - SMS_CAPTURE
Usse - SECURITY
Ussh - SHELL
Ussi - SENDINPUT
Ussm - SMS
Usss - SMS_STRING
Usst - SCROLLTRACK
Ussw - SWP
Ussy - SYSTEM
Ustd - TRACKDESKTOP
Usti - THREADINFO
Ustm - TIMER
Ustr - TERMINAL
Ustx - TEXT
Usty - TEXT2
Usub - UNICODEBUFFER
Usvi - VISRGN
Usvl - VWPL
Uswd - WINDOW
Uswe - WINEVENT
Uswl - WINDOWLIST
Uswo - WOWTDB
Uswp - WOWPROCESSINFO
Usws - WOWSEL
Uswt - WOWTHREADINFO
uszz - MAX

Vad  - Mm virtual address descriptors
Vadl - Mm virtual address descriptors (long)
VadS - Mm virtual address descriptors (short)
VidL - VideoPort Allocation List (FDO_EXTENSION)
VidR - VideoPort Allocation on behalf of Miniport
VDM  - ntos\vdm

VoS? -  VolSnap (Volume Snapshot Driver)
VoSa -      Application information allocations
VoSb -      Buffer allocations
VoSc -      Snapshot context allocations
VoSd -      Diff area volume allocations
VoSf -      Diff area file allocations
VoSh -      Bit history allocations
VoSi -      Io status block allocations
VoSm -      Bitmap allocations
VoSo -      Old heap entry allocations
VoSp -      Pnp id allocations
VoSr -      Device relations allocations
VoSs -      Short term allocations
VoSt -      Temp table allocations
VoSw -      Work queue allocations
VoSx -      Dispatch context allocations

Vpb  - Io, vpb's

Vprt - ntos\video\port
VraP - parallel class driver
Vtfd - Font file/context

Wait - NtWaitForMultipleObjects

Wan? - NdisWan Tags (PPP Framing module for Remote Access)
WanA    - BundleCB
WanB    - ProtocolCB/LinkCB
WanC    - DataDesc
WanD    - WanRequest
WanE    - LoopbackDesc
WanG    - MiniportCB
WanH    - OpenCB
WanI    - IoPacket
WanJ    - LineUpInfo
WanK    - Unicode String Buffer
WanL    - Protocol Table
WanM    - Connection Table
WanN    - NdisPacketPool Desc
WanQ    - DataBuffer
WanR    - WanPacket
WanS    - AfCB/SapCB/VcCB
WanT    - Transform Driver
WanV    - RC4 Encryption Context
WanW    - SHA Encryption
WanX    - Send Compression Context
WanY    - Recv Compression Context
WanZ    - Protocol Specific Info

Wdm  - WDM
WDMA - WDM Audio

Wmii - Wmi InstId chunks
Wmij - Wmi GuidMaps
Wmim - Wmi KM to UM Notification Buffers
Wmin - Wmi Notification Slot Chunks
Wmip - Wmi General purpose allocation
Wmis - Wmi SysId allocations
Wmit - Wmi Trace
Wmiw - Wmi Notification Waiting Buffers, in paged queue waiting for IOCTL
Wmiz - Wmi MCA Insertions debug code

WmiA - Wmi ACPI mapper
WmiC - Wmi Create Pump Thread Work Item
WmiD - Wmi Registration DataSouce
WmiI - Wmi Instance Names
WmiL - WmiLIb
WmiN - Wmi Notification Notification Packet
WmiR - Wmi Registration info blocks

WmDS - Wmi DataSource chunks
WmGE - Wmi GuidEntry chunks
WmIS - Wmi InstanceSet chunks
WmMR - Wmi MofResouce chunks

WMca - WMI MCA Handling

Wrp? - WanArp Tags (ARP module for Remote Access)

Wrpa - WAN_ADAPTER_TAG
Wrpi - WAN_INTERFACE_TAG
Wrpr - WAN_REQUEST_TAG
Wrps - WAN_STRING_TAG
Wrpc - WAN_CONN_TAG
Wrpw - WAN_PACKET_TAG
Wrpf - FREE_TAG (checked builds only)
Wrpd - WAN_DATA_TAG
Wrph - WAN_HEADER_TAG
Wrpn - WAN_NOTIFICATION_TAG

WvCy - WDM Audio WaveCyc port
WvPc - WDM Audio WavePCI port

XWan - ndis\usrwan allocations

Xtra - EXIFS Extra Create

ysd8 - User DDE8
ysd9 - User DDE9
ysda - User DDEa
ysdb - User DDEb
ysdc - User DDEc
ysdd - User DDEd

ZsaB - EXIFS ZeroBlock
