/*
Test part variable fetch code
*/

#include "ncdispatch.h"
#include "nctestserver.h"
#include "netcdf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The DDS in netcdf classic form is as follows: 
netcdf ingrid {
dimensions:
	ISTA = 35 ;
	IZ = 44 ;
variables:
	int ISTA(ISTA) ;
	float IZ(IZ) ;
	float v3H(ISTA, IZ) ;
}
*/

#define PARAMS ""
#define DTSTEST "/ingrid"
#define VAR "v3H"
#define ISTA 35
#define IZ 44

#define RANK 2

/* Define the delta for float equality */
#define DELTA 0.005

#define ERRCODE 2
#define ERR(e)                             \
  {                                        \
    printf("Error: %s\n", nc_strerror(e)); \
    exit(ERRCODE);                         \
  }

#undef DEBUG

/* Setup an odometer */
typedef struct Odom {
  int rank;
  size_t *index;
  size_t *stop;
  size_t *start;
  size_t *count;
} Odom;

#if 0
static float targetwhole[ISTA*IZ];
#endif
static float targetpart[ISTA * IZ];
static float target_content[ISTA * IZ];

static Odom *odom_create(int rank);
static void odom_reclaim(Odom *odom);
static void odom_set(Odom *odom, size_t *start, size_t *count);
static int odom_more(Odom *odom);
static int odom_incr(Odom *odom);
static size_t odom_count(Odom *odom);

static size_t subslice(int rank, size_t *count, int startdim);
static int check(float *, size_t *start, size_t *count);

/* Define whole variable start/count */
static size_t start0[RANK] = {0, 0};
static size_t count0[RANK] = {ISTA, IZ};

static int
floateq(float f1, float f2) {
  float diff = f1 - f2;
  if (diff >= 0 && diff < DELTA) return 1;
  if (diff < 0 && diff > -DELTA) return 1;
  return 0;
}

void dump(float *source, size_t start, size_t count) {
#ifdef DEBUG
  int i;
  printf("start=%lu count=%lu\n", (unsigned long)start, (unsigned long)count);
  for (i = 0; i < count; i++) {
    printf(" %g", source[start + i]);
    if ((i % 6) == 5) printf("\n");
  }
  printf("\n");
#endif
}

int main() {
  int ncid, varid;
  int retval;
  size_t start[RANK];
  size_t count[RANK];
  size_t offset;
  char url[4096];
  char *svc = NULL;

  /* Find Test Server */
  svc = nc_findtestserver("dts", 0, REMOTETESTSERVERS);

  if (svc == NULL) {
    fprintf(stderr, "Cannot locate test server\n");
    exit(0);
  }
  strncpy(url, PARAMS, sizeof(url));
  strlcat(url, svc, sizeof(url));
  strlcat(url, DTSTEST, sizeof(url));
  free(svc);

  printf("test_partvar: url=%s\n", url);

  if ((retval = nc_open(url, NC_NOWRITE, &ncid)))
    ERR(retval);

  if ((retval = nc_inq_varid(ncid, VAR, &varid)))
    ERR(retval);

  /* read piece by piece */
  memcpy(start, start0, sizeof(start0));
  memcpy(count, count0, sizeof(count0));
  count[0] = 1;
  memset((void *)targetpart, 0, sizeof(targetpart));
  for (offset = 0; start[0] < count0[0]; start[0]++) {
    size_t nslice;
    float *fpos = (float *)targetpart;
    fpos += offset;
    if ((retval = nc_get_vara_float(ncid, varid, start, count, fpos)))
      ERR(retval);
    nslice = subslice(RANK, count, 1);
    offset += nslice;
  }

#ifdef DEBUG
  dump((float *)targetpart, 0, ISTA * IZ);
#endif

  /* validate the part var */
  if (!check(targetpart, start0, count0)) goto fail;

  if ((retval = nc_close(ncid)))
    ERR(retval);

  printf("*** PASS\n");
  return 0;
fail:
  printf("*** FAIL\n");
  return 1;
}

static size_t
subslice(int rank, size_t *count, int startdim) {
  int i;
  size_t offset = 1;
  for (i = startdim; i < rank; i++)
    offset *= count[i];
  return offset;
}

static int
check(float *target, size_t *start, size_t *count) {
  int ok          = 1;
  Odom *odom      = odom_create(RANK);
  float *result   = (float *)target;
  float *expected = (float *)target_content;
  odom_set(odom, start, count);
  while (odom_more(odom)) {
    size_t offset = odom_count(odom);
    int eq        = floateq(result[offset], expected[offset]);
    if (eq == 0) {
      fprintf(stderr, "fail: result[%lu] = %f ; expected[%lu] = %f\n",
      (unsigned long)offset, result[offset], (unsigned long)offset, expected[offset]);
      ok = 0;
    }
    odom_incr(odom);
  }
  odom_reclaim(odom);
  return ok;
}

static Odom *
odom_create(int rank) {
  Odom *odom = (Odom *)malloc(sizeof(Odom));
  /* Init the odometer */
  odom->rank  = rank;
  odom->index = (size_t *)calloc(sizeof(size_t) * rank, 1);
  odom->stop  = (size_t *)calloc(sizeof(size_t) * rank, 1);
  odom->start = (size_t *)calloc(sizeof(size_t) * rank, 1);
  odom->count = (size_t *)calloc(sizeof(size_t) * rank, 1);
  return odom;
}

static void
odom_reclaim(Odom *odom) {
  free(odom->index);
  free(odom->stop);
  free(odom->start);
  free(odom->count);
  free(odom);
}

static void
odom_set(Odom *odom, size_t *start, size_t *count) {
  int i;
  /* Init the odometer */
  for (i = 0; i < odom->rank; i++) {
    odom->start[i] = start[i];
    odom->count[i] = count[i];
  }
  for (i = 0; i < odom->rank; i++) {
    odom->index[i] = odom->start[i];
    odom->stop[i]  = odom->start[i] + odom->count[i];
  }
}

static int
odom_more(Odom *odom) {
  return (odom->index[0] < odom->stop[0] ? 1 : 0);
}

static int
odom_incr(Odom *odom) {
  int i; /* do not make unsigned */
  if (odom->rank == 0) return 0;
  for (i = odom->rank - 1; i >= 0; i--) {
    odom->index[i]++;
    if (odom->index[i] < odom->stop[i]) break;
    if (i == 0) return 0;            /* leave the 0th entry if it overflows*/
    odom->index[i] = odom->start[i]; /* reset this position*/
  }
  return 1;
}

/* Convert current dapodometer settings to a single integer count*/
static size_t
odom_count(Odom *odom) {
  int i;
  size_t offset = 0;
  for (i = 0; i < odom->rank; i++) {
    offset *= odom->count[i];
    offset += odom->index[i];
  }
  return offset;
}

/* Capture the complete set of data */
static float target_content[ISTA * IZ] = {
0, 0.009999833, 0.01999867, 0.0299955, 0.03998933, 0.04997917, 0.059964,
0.06994285, 0.0799147, 0.08987855, 0.09983341, 0.1097783, 0.1197122,
0.1296341, 0.1395431, 0.1494381, 0.1593182, 0.1691823, 0.1790296,
0.1888589, 0.1986693, 0.2084599, 0.2182296, 0.2279775, 0.2377026,
0.247404, 0.2570806, 0.2667314, 0.2763557, 0.2859522, 0.2955202,
0.3050586, 0.3145666, 0.324043, 0.3334871, 0.3428978, 0.3522742,
0.3616154, 0.3709205, 0.3801884, 0.3894183, 0.3986093, 0.4077604,
0.4168708,
0.4259395, 0.4349655, 0.4439481, 0.4528863, 0.4617792, 0.4706259,
0.4794255, 0.4881772, 0.4968801, 0.5055333, 0.514136, 0.5226873,
0.5311862, 0.539632, 0.5480239, 0.556361, 0.5646425, 0.5728675,
0.5810351, 0.5891448, 0.5971954, 0.6051864, 0.6131169, 0.620986,
0.628793, 0.6365372, 0.6442177, 0.6518338, 0.6593847, 0.6668696,
0.6742879, 0.6816388, 0.6889215, 0.6961352, 0.7032794, 0.7103533,
0.7173561, 0.7242872, 0.7311459, 0.7379314, 0.7446431, 0.7512804,
0.7578425, 0.764329,
0.7707389, 0.7770718, 0.7833269, 0.7895038, 0.7956016, 0.8016199,
0.8075581, 0.8134155, 0.8191916, 0.8248857, 0.8304974, 0.836026,
0.841471, 0.8468319, 0.852108, 0.857299, 0.8624042, 0.8674232, 0.8723555,
0.8772005, 0.8819578, 0.8866269, 0.8912073, 0.8956987, 0.9001005,
0.9044122, 0.9086335, 0.912764, 0.9168031, 0.9207506, 0.924606, 0.928369,
0.9320391, 0.935616, 0.9390994, 0.9424888, 0.945784, 0.9489846,
0.9520903, 0.9551008, 0.9580159, 0.960835, 0.9635582, 0.966185,
0.9687151, 0.9711484, 0.9734845, 0.9757234, 0.9778646, 0.979908, 0.9818535,
0.9837008, 0.9854497, 0.9871001, 0.9886518, 0.9901046, 0.9914584,
0.992713, 0.9938684, 0.9949244, 0.9958808, 0.9967378, 0.997495,
0.9981525, 0.9987102, 0.9991679, 0.9995258, 0.9997838, 0.9999417,
0.9999997, 0.9999576, 0.9998156, 0.9995736, 0.9992316, 0.9987897,
0.998248, 0.9976064, 0.996865, 0.996024, 0.9950833, 0.9940432, 0.9929036,
0.9916648, 0.9903268, 0.9888898, 0.9873539, 0.9857192, 0.983986,
0.9821543, 0.9802245, 0.9781966, 0.9760709, 0.9738476, 0.971527, 0.9691091,
0.9665944, 0.963983, 0.9612752, 0.9584713, 0.9555715, 0.9525762,
0.9494856, 0.9463001, 0.9430199, 0.9396455, 0.9361771, 0.932615,
0.9289597, 0.9252115, 0.9213708, 0.917438, 0.9134133, 0.9092974,
0.9050906, 0.9007932, 0.8964058, 0.8919287, 0.8873624, 0.8827074,
0.8779641, 0.873133, 0.8682146, 0.8632094, 0.8581178, 0.8529405,
0.8476778, 0.8423305, 0.8368988, 0.8313835, 0.825785, 0.8201039, 0.8143409,
0.8084964, 0.8025711, 0.7965655, 0.7904802, 0.7843159, 0.7780732,
0.7717527, 0.7653549, 0.7588807, 0.7523306, 0.7457052, 0.7390053,
0.7322314, 0.7253844, 0.7184648, 0.7114733, 0.7044108, 0.6972777,
0.690075, 0.6828032, 0.6754632, 0.6680556, 0.6605812, 0.6530408,
0.645435, 0.6377647, 0.6300306, 0.6222336, 0.6143743, 0.6064535,
0.5984721, 0.5904309, 0.5823306, 0.5741721, 0.5659562, 0.5576837,
0.5493554, 0.5409722, 0.5325349, 0.5240443, 0.5155014, 0.5069069,
0.4982616, 0.4895666,
0.4808226, 0.4720306, 0.4631913, 0.4543057, 0.4453746, 0.4363991,
0.4273799, 0.4183179, 0.4092142, 0.4000695, 0.3908848, 0.381661,
0.372399, 0.3630998, 0.3537644, 0.3443935, 0.3349881, 0.3255493,
0.316078, 0.306575, 0.2970414, 0.287478, 0.2778859, 0.2682661, 0.2586193,
0.2489468, 0.2392493, 0.229528, 0.2197836, 0.2100173, 0.20023, 0.1904227,
0.1805963, 0.1707518, 0.1608903, 0.1510127, 0.14112, 0.1312132,
0.1212933, 0.1113612, 0.101418, 0.09146464, 0.08150215, 0.07153151,
0.06155372, 0.05156977, 0.04158066, 0.0315874, 0.02159098, 0.01159239,
0.001592653, -0.008407247, -0.01840631, -0.02840353, -0.0383979,
-0.04838844, -0.05837414, -0.068354, -0.07832703, -0.08829223,
-0.09824859, -0.1081951, -0.1181309, -0.1280548, -0.1379659, -0.1478632,
-0.1577457, -0.1676124, -0.1774624, -0.1872947, -0.1971082, -0.206902,
-0.2166751, -0.2264265, -0.2361553, -0.2458605, -0.2555411, -0.2651961,
-0.2748247, -0.2844257, -0.2939983, -0.3035415, -0.3130544, -0.3225359,
-0.3319852, -0.3414013, -0.3507832, -0.3601301,
-0.369441, -0.3787149, -0.3879509, -0.3971482, -0.4063057, -0.4154226,
-0.424498, -0.4335309, -0.4425204, -0.4514658, -0.4603659, -0.46922,
-0.4780273, -0.4867867, -0.4954974, -0.5041586, -0.5127693, -0.5213288,
-0.5298361, -0.5382905, -0.5466911, -0.5550369, -0.5633273, -0.5715613,
-0.5797382, -0.5878571, -0.5959172, -0.6039178, -0.6118579, -0.6197369,
-0.6275538, -0.635308, -0.6429988, -0.6506251, -0.6581865, -0.665682,
-0.673111, -0.6804726, -0.6877661, -0.694991, -0.7021463, -0.7092314,
-0.7162456, -0.7231881,
-0.7300584, -0.7368556, -0.7435791, -0.7502283, -0.7568025, -0.763301,
-0.7697231, -0.7760683, -0.7823359, -0.7885253, -0.7946358, -0.8006668,
-0.8066177, -0.8124881, -0.8182771, -0.8239843, -0.8296092, -0.835151,
-0.8406094, -0.8459837, -0.8512734, -0.856478, -0.8615969, -0.8666297,
-0.8715758, -0.8764347, -0.881206, -0.8858892, -0.8904838, -0.8949894,
-0.8994054, -0.9037315, -0.9079673, -0.9121122, -0.9161659, -0.920128,
-0.9239982, -0.9277759, -0.9314608, -0.9350526, -0.9385508, -0.9419553,
-0.9452655, -0.9484812,
-0.9516021, -0.9546278, -0.957558, -0.9603925, -0.963131, -0.965773,
-0.9683186, -0.9707673, -0.973119, -0.9753733, -0.9775301, -0.9795892,
-0.9815503, -0.9834132, -0.9851778, -0.9868439, -0.9884112, -0.9898798,
-0.9912494, -0.9925198, -0.993691, -0.9947628, -0.9957352, -0.996608,
-0.9973811, -0.9980544, -0.998628, -0.9991017, -0.9994755, -0.9997494,
-0.9999232, -0.9999971, -0.999971, -0.9998449, -0.9996188, -0.9992928,
-0.9988668, -0.998341, -0.9977152, -0.9969898, -0.9961646, -0.9952399,
-0.9942155, -0.9930918,
-0.9918687, -0.9905465, -0.9891253, -0.9876051, -0.9859861, -0.9842686,
-0.9824526, -0.9805384, -0.9785261, -0.976416, -0.9742082, -0.9719031,
-0.9695007, -0.9670014, -0.9644054, -0.9617129, -0.9589243, -0.9560397,
-0.9530596, -0.9499842, -0.9468138, -0.9435487, -0.9401892, -0.9367357,
-0.9331886, -0.9295481, -0.9258147, -0.9219887, -0.9180705, -0.9140605,
-0.9099591, -0.9057667, -0.9014837, -0.8971105, -0.8926477, -0.8880956,
-0.8834547, -0.8787254, -0.8739083, -0.8690037, -0.8640123, -0.8589345,
-0.8537708, -0.8485217,
-0.8431877, -0.8377695, -0.8322675, -0.8266822, -0.8210142, -0.8152642,
-0.8094327, -0.8035201, -0.7975273, -0.7914547, -0.7853029, -0.7790727,
-0.7727645, -0.766379, -0.7599169, -0.7533789, -0.7467654, -0.7400773,
-0.7333152, -0.7264798, -0.7195717, -0.7125916, -0.7055403, -0.6984185,
-0.6912268, -0.683966, -0.6766368, -0.6692399, -0.6617761, -0.6542461,
-0.6466507, -0.6389906, -0.6312667, -0.6234795, -0.6156301, -0.6077191,
-0.5997473, -0.5917156, -0.5836247, -0.5754754, -0.5672686, -0.559005,
-0.5506855, -0.542311,
-0.5338823, -0.5254001, -0.5168654, -0.5082791, -0.4996419, -0.4909547,
-0.4822185, -0.473434, -0.4646022, -0.4557239, -0.4468001, -0.4378315,
-0.4288192, -0.419764, -0.4106669, -0.4015286, -0.3923502, -0.3831326,
-0.3738767, -0.3645833, -0.3552535, -0.3458883, -0.3364884, -0.3270548,
-0.3175886, -0.3080906, -0.2985618, -0.2890031, -0.2794155, -0.2698,
-0.2601575, -0.250489, -0.2407954, -0.2310778, -0.2213371, -0.2115742,
-0.2017901, -0.1919859, -0.1821625, -0.1723209, -0.162462, -0.1525869,
-0.1426965, -0.1327919,
-0.122874, -0.1129438, -0.1030023, -0.0930505, -0.0830894, -0.07311999,
-0.06314328, -0.05316024, -0.04317189, -0.03317922, -0.02318323,
-0.01318493, -0.003185302, 0.00681464, 0.0168139, 0.02681148, 0.03680638,
0.0467976, 0.05678413, 0.06676499, 0.07673918, 0.08670568, 0.09666352,
0.1066117, 0.1165492, 0.1264751, 0.1363883, 0.1462878, 0.1561728,
0.1660421, 0.1758948, 0.18573, 0.1955465, 0.2053435, 0.21512, 0.2248749,
0.2346074, 0.2443164, 0.254001, 0.2636602, 0.273293, 0.2828985,
0.2924757, 0.3020236,
0.3115413, 0.321028, 0.3304825, 0.3399039, 0.3492913, 0.3586439, 0.3679605,
0.3772404, 0.3864825, 0.395686, 0.4048499, 0.4139734, 0.4230554,
0.4320951, 0.4410917, 0.4500441, 0.4589515, 0.467813, 0.4766277,
0.4853948, 0.4941134, 0.5027825, 0.5114013, 0.519969, 0.5284848,
0.5369476, 0.5453568, 0.5537114, 0.5620106, 0.5702537, 0.5784398,
0.5865679, 0.5946375, 0.6026475, 0.6105974, 0.6184861, 0.626313,
0.6340773, 0.6417782, 0.6494148, 0.6569866, 0.6644927, 0.6719322,
0.6793047,
0.6866091, 0.693845, 0.7010114, 0.7081077, 0.7151332, 0.7220873, 0.728969,
0.735778, 0.7425133, 0.7491744, 0.7557605, 0.7622711, 0.7687054,
0.7750629, 0.7813429, 0.7875448, 0.7936679, 0.7997116, 0.8056753,
0.8115585, 0.8173606, 0.8230809, 0.8287189, 0.834274, 0.8397457,
0.8451334, 0.8504366, 0.8556548, 0.8607874, 0.8658339, 0.8707939,
0.8756667, 0.880452, 0.8851492, 0.889758, 0.8942778, 0.8987081,
0.9030486, 0.9072987, 0.9114581, 0.9155264, 0.9195032, 0.9233879,
0.9271804,
0.9308801, 0.9344868, 0.938, 0.9414194, 0.9447446, 0.9479754, 0.9511114,
0.9541523, 0.9570977, 0.9599475, 0.9627013, 0.9653587, 0.9679196,
0.9703838, 0.972751, 0.9750208, 0.9771932, 0.9792678, 0.9812445,
0.9831231, 0.9849033, 0.9865851, 0.9881682, 0.9896525, 0.9910379,
0.9923241, 0.9935111, 0.9945988, 0.995587, 0.9964756, 0.9972646,
0.9979539, 0.9985433, 0.999033, 0.9994227, 0.9997125, 0.9999022,
0.9999921, 0.9999819, 0.9998717, 0.9996616, 0.9993514, 0.9989414,
0.9984314,
0.9978216, 0.997112, 0.9963027, 0.9953938, 0.9943853, 0.9932774, 0.9920702,
0.9907638, 0.9893582, 0.9878538, 0.9862506, 0.9845487, 0.9827484,
0.9808499, 0.9788532, 0.9767586, 0.9745664, 0.9722767, 0.9698898,
0.9674059, 0.9648253, 0.9621482, 0.9593748, 0.9565055, 0.9535406,
0.9504804, 0.9473251, 0.944075, 0.9407306, 0.937292, 0.9337597,
0.9301341, 0.9264155, 0.9226042, 0.9187007, 0.9147053, 0.9106184,
0.9064404, 0.9021719, 0.897813, 0.8933644, 0.8888265, 0.8841997, 0.8794845,
0.8746814, 0.8697907, 0.8648131, 0.859749, 0.8545989, 0.8493634, 0.8440429,
0.8386381, 0.8331493, 0.8275773, 0.8219225, 0.8161855, 0.8103669,
0.8044672, 0.7984871, 0.7924272, 0.786288, 0.7800702, 0.7737743,
0.7674012, 0.7609512, 0.7544252, 0.7478237, 0.7411475, 0.7343971,
0.7275733, 0.7206767, 0.7137081, 0.7066681, 0.6995574, 0.6923768,
0.685127, 0.6778086, 0.6704224, 0.6629692, 0.6554497, 0.6478647,
0.6402149, 0.6325011, 0.624724, 0.6168844, 0.6089832, 0.601021, 0.5929987,
0.5849172, 0.5767772, 0.5685794, 0.5603248, 0.5520142, 0.5436484,
0.5352283, 0.5267546, 0.5182282, 0.50965, 0.5010208, 0.4923416,
0.4836131, 0.4748363, 0.4660119, 0.457141, 0.4482243, 0.4392629,
0.4302575, 0.421209, 0.4121185, 0.4029867, 0.3938147, 0.3846032,
0.3753533, 0.3660659, 0.3567419, 0.3473822, 0.3379877, 0.3285595,
0.3190984, 0.3096054, 0.3000814, 0.2905274, 0.2809443, 0.2713332,
0.261695, 0.2520306, 0.2423409, 0.232627, 0.2228899, 0.2131305,
0.2033498, 0.1935487,
0.1837283, 0.1738895, 0.1640333, 0.1541607, 0.1442727, 0.1343703,
0.1244544, 0.1145261, 0.1045863, 0.09463613, 0.08467644, 0.07470829,
0.06473266, 0.05475057, 0.044763, 0.03477095, 0.02477542, 0.01477742,
0.004777943, -0.005222016, -0.01522145, -0.02521937, -0.03521476,
-0.04520663, -0.05519398, -0.06517581, -0.07515112, -0.08511892,
-0.09507821, -0.105028, -0.1149673, -0.124895, -0.1348103, -0.1447121,
-0.1545995, -0.1644713, -0.1743268, -0.1841648, -0.1939844, -0.2037845,
-0.2135644, -0.2233228, -0.2330589, -0.2427717,
-0.2524603, -0.2621236, -0.2717606, -0.2813705, -0.2909523, -0.300505,
-0.3100276, -0.3195192, -0.3289789, -0.3384056, -0.3477986, -0.3571567,
-0.3664791, -0.3757649, -0.3850131, -0.3942228, -0.4033931, -0.4125231,
-0.4216118, -0.4306583, -0.4396617, -0.4486212, -0.4575359, -0.4664048,
-0.475227, -0.4840018, -0.4927281, -0.5014051, -0.5100321, -0.518608,
-0.527132, -0.5356033, -0.5440211, -0.5523845, -0.5606926, -0.5689447,
-0.5771399, -0.5852773, -0.5933563, -0.6013759, -0.6093353, -0.6172339,
-0.6250706, -0.6328449,
-0.640556, -0.6482029, -0.6557851, -0.6633016, -0.6707519, -0.678135,
-0.6854504, -0.6926972, -0.6998747, -0.7069823, -0.7140191, -0.7209845,
-0.7278779, -0.7346984, -0.7414455, -0.7481185, -0.7547166, -0.7612393,
-0.7676858, -0.7740556, -0.7803479, -0.7865623, -0.792698, -0.7987544,
-0.8047309, -0.810627, -0.816442, -0.8221753, -0.8278264, -0.8333948,
-0.8388798, -0.844281, -0.8495977, -0.8548294, -0.8599757, -0.865036,
-0.8700097, -0.8748965, -0.8796958, -0.884407, -0.8890299, -0.8935639,
-0.8980085, -0.9023633,
-0.9066279, -0.9108018, -0.9148846, -0.918876, -0.9227754, -0.9265826,
-0.9302971, -0.9339186, -0.9374467, -0.9408811, -0.9442213, -0.9474672,
-0.9506183, -0.9536743, -0.956635, -0.9595, -0.9622691, -0.9649419,
-0.9675183, -0.9699979, -0.9723805, -0.9746658, -0.9768537, -0.9789439,
-0.9809362, -0.9828305, -0.9846264, -0.9863239, -0.9879227, -0.9894227,
-0.9908239, -0.9921259, -0.9933288, -0.9944322, -0.9954363, -0.9963408,
-0.9971456, -0.9978508, -0.9984561, -0.9989617, -0.9993673, -0.999673,
-0.9998787, -0.9999844,
-0.9999902, -0.9998959, -0.9997017, -0.9994075, -0.9990134, -0.9985193,
-0.9979254, -0.9972317, -0.9964383, -0.9955452, -0.9945526, -0.9934605,
-0.9922691, -0.9909785, -0.9895887, -0.9881001, -0.9865125, -0.9848264,
-0.9830417, -0.9811588, -0.9791777, -0.9770988, -0.9749221, -0.9726479,
-0.9702765, -0.967808, -0.9652427, -0.962581, -0.959823, -0.956969,
-0.9540192, -0.9509742, -0.947834, -0.944599, -0.9412695, -0.9378459,
-0.9343286, -0.9307178, -0.9270139, -0.9232174, -0.9193285, -0.9153477,
-0.9112754, -0.9071119,
-0.9028577, -0.8985133, -0.894079, -0.8895552, -0.8849425, -0.8802414,
-0.8754522, -0.8705754, -0.8656116, -0.8605613, -0.8554249, -0.8502029,
-0.844896, -0.8395045, -0.8340291, -0.8284702, -0.8228286, -0.8171046,
-0.811299, -0.8054122, -0.7994449, -0.7933976, -0.787271, -0.7810657,
-0.7747822, -0.7684214, -0.7619836, -0.7554696, -0.7488801, -0.7422158,
-0.7354771, -0.728665, -0.7217799, -0.7148228, -0.7077941, -0.7006946,
-0.6935251, -0.6862862, -0.6789787, -0.6716033, -0.6641607, -0.6566517,
-0.6490771, -0.6414375,
-0.6337339, -0.6259668, -0.6181371, -0.6102456, -0.6022931, -0.5942804,
-0.5862082, -0.5780774, -0.5698889, -0.5616433, -0.5533416, -0.5449845,
-0.5365729, -0.5281077, -0.5195897, -0.5110196, -0.5023986, -0.4937272,
-0.4850065, -0.4762373, -0.4674205, -0.4585569, -0.4496475, -0.4406931,
-0.4316946, -0.422653, -0.4135691, -0.4044438, -0.3952781, -0.3860729,
-0.3768291, -0.3675475, -0.3582293, -0.3488752, -0.3394862, -0.3300633,
-0.3206073, -0.3111193, -0.3016002, -0.292051, -0.2824725, -0.2728658,
-0.2632318, -0.2535715,
-0.2438858, -0.2341757, -0.2244422, -0.2146863, -0.2049089, -0.195111,
-0.1852936, -0.1754577, -0.1656042, -0.1557341, -0.1458485, -0.1359483,
-0.1260345, -0.1161081, -0.1061701, -0.09622151, -0.08626327, -0.0762964,
-0.06632189, -0.05634077, -0.046354, -0.0363626, -0.02636756,
-0.01636988, -0.006370571, 0.003629378, 0.01362896, 0.02362719,
0.03362305, 0.04361555, 0.05360368, 0.06358646, 0.07356288, 0.08353194,
0.09349265, 0.103444, 0.113385, 0.1233147, 0.133232, 0.1431361,
0.1530258, 0.1629002, 0.1727583, 0.1825991,
0.1924217, 0.2022251, 0.2120082, 0.2217701, 0.2315098, 0.2412264,
0.2509189, 0.2605863, 0.2702276, 0.2798419, 0.2894282, 0.2989855,
0.308513, 0.3180096, 0.3274744, 0.3369065, 0.3463049, 0.3556686,
0.3649968, 0.3742885, 0.3835427, 0.3927587, 0.4019353, 0.4110717,
0.420167, 0.4292203, 0.4382307, 0.4471973, 0.4561191, 0.4649954,
0.4738251, 0.4826075, 0.4913416, 0.5000265, 0.5086614, 0.5172455,
0.5257779, 0.5342577, 0.5426841, 0.5510561, 0.5593731, 0.5676342,
0.5758385, 0.5839852,
0.5920735, 0.6001026, 0.6080717, 0.61598, 0.6238267, 0.631611, 0.6393321,
0.6469893, 0.6545818, 0.6621089, 0.6695698, 0.6769636, 0.6842899,
0.6915476, 0.6987363, 0.705855, 0.7129031, 0.71988, 0.7267848, 0.7336171,
0.7403759, 0.7470607, 0.7536708, 0.7602055, 0.7666642, 0.7730463,
0.779351, 0.7855778, 0.7917261, 0.7977951, 0.8037844, 0.8096933,
0.8155213, 0.8212677, 0.826932, 0.8325136, 0.8380119, 0.8434264,
0.8487566, 0.8540019, 0.8591618, 0.8642358, 0.8692234, 0.8741241,
0.8789373, 0.8836626, 0.8882996, 0.8928478, 0.8973066, 0.9016758,
0.9059547, 0.9101431, 0.9142405, 0.9182464, 0.9221606, 0.9259824,
0.9297118, 0.9333481, 0.9368911, 0.9403404, 0.9436957, 0.9469566,
0.9501228, 0.953194, 0.9561699, 0.9590501, 0.9618345, 0.9645227,
0.9671144, 0.9696094, 0.9720075, 0.9743084, 0.9765118, 0.9786175,
0.9806255, 0.9825354, 0.9843469, 0.9860601, 0.9876747, 0.9891905,
0.9906074, 0.9919252, 0.9931438, 0.9942631, 0.995283, 0.9962034,
0.9970241, 0.9977452,
0.9983664, 0.9988878, 0.9993094, 0.999631, 0.9998527, 0.9999743, 0.999996,
0.9999177, 0.9997393, 0.9994611, 0.9990828, 0.9986047, 0.9980267,
0.9973488, 0.9965713, 0.9956941, 0.9947174, 0.9936411, 0.9924655,
0.9911907, 0.9898167, 0.9883437, 0.9867719, 0.9851015, 0.9833325,
0.9814652, 0.9794998, 0.9774364, 0.9752753, 0.9730166, 0.9706606,
0.9682076, 0.9656578, 0.9630114, 0.9602687, 0.9574299, 0.9544954,
0.9514655, 0.9483404, 0.9451205, 0.9418061, 0.9383975, 0.934895, 0.9312991,
0.9276101, 0.9238282, 0.9199541, 0.9159878, 0.9119301, 0.9077811,
0.9035413, 0.8992112, 0.8947912, 0.8902817, 0.8856831, 0.880996,
0.8762208, 0.871358, 0.866408, 0.8613714, 0.8562487, 0.8510403,
0.8457468, 0.8403688, 0.8349067, 0.8293611, 0.8237326, 0.8180218,
0.8122291, 0.8063552, 0.8004007, 0.7943661, 0.7882521, 0.7820593,
0.7757882, 0.7694396, 0.763014, 0.7565122, 0.7499346, 0.7432821,
0.7365553, 0.7297548, 0.7228814, 0.7159356, 0.7089183, 0.70183,
0.6946716, 0.6874437,
0.6801471, 0.6727825, 0.6653506, 0.6578521, 0.6502879, 0.6426586,
0.6349651, 0.627208, 0.6193883, 0.6115066, 0.6035637, 0.5955606,
0.5874978, 0.5793763, 0.5711969, 0.5629603, 0.5546675, 0.5463191,
0.5379162, 0.5294595, 0.5209498, 0.512388, 0.503775, 0.4951116,
0.4863987, 0.4776371, 0.4688278, 0.4599716, 0.4510695, 0.4421222,
0.4331307, 0.4240958, 0.4150186, 0.4058999, 0.3967406, 0.3875416,
0.3783038, 0.3690283, 0.3597158, 0.3503673, 0.3409838, 0.3315663,
0.3221155, 0.3126326};
