#include <nc_tests.h>
#include "err_macros.h"
#include "netcdf.h"
#include <signal.h>

#define FILE_NAME "tst_atts1.nc"
#define VAR1_NAME "Horace_Rumpole"
#define VAR2_NAME "Claude_Erskine-Brown"
#define DIM1_NAME "Old_Bailey_case_number"
#define DIM1_LEN 10
#define DIM2_NAME "occupancy_in_chambers"
#define DIM2_LEN 15
#define ATT_TEXT_NAME "Speech_to_Jury"

char speech[] = "Once more unto the breach, dear friends, once more;\n\
Or close the wall up with our English dead.\n\
In peace there's nothing so becomes a man\n\
As modest stillness and humility:\n\
But when the blast of war blows in our ears,\n\
Then imitate the action of the tiger;\n\
Stiffen the sinews, summon up the blood,\n\
Disguise fair nature with hard-favour'd rage;\n\
Then lend the eye a terrible aspect;\n\
Let pry through the portage of the head\n\
Like the brass cannon; let the brow o'erwhelm it\n\
As fearfully as doth a galled rock\n\
O'erhang and jutty his confounded base,\n\
Swill'd with the wild and wasteful ocean.\n\
Now set the teeth and stretch the nostril wide,\n\
Hold hard the breath and bend up every spirit\n\
To his full height. On, on, you noblest English.\n\
Whose blood is fet from fathers of war-proof!\n\
Fathers that, like so many Alexanders,\n\
Have in these parts from morn till even fought\n\
And sheathed their swords for lack of argument:\n\
Dishonour not your mothers; now attest\n\
That those whom you call'd fathers did beget you.\n\
Be copy now to men of grosser blood,\n\
And teach them how to war. And you, good yeoman,\n\
Whose limbs were made in England, show us here\n\
The mettle of your pasture; let us swear\n\
That you are worth your breeding; which I doubt not;\n\
For there is none of you so mean and base,\n\
That hath not noble lustre in your eyes.\n\
I see you stand like greyhounds in the slips,\n\
Straining upon the start. The game's afoot:\n\
Follow your spirit, and upon this charge\n\
Cry 'God for Harry, England, and Saint George!'";

int
main(int argc, char **argv)
{
      int ncid, varid, dimids[2];
      nc_type att_type;
      size_t att_len;
      int i, v;

      char *speech_in;

      /* Create a file with two vars, attaching to each an attribute of
       * each type. */
      if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;
      if (nc_def_dim(ncid, DIM1_NAME, DIM1_LEN, &dimids[0])) ERR;
      if (nc_def_dim(ncid, DIM2_NAME, DIM2_LEN, &dimids[1])) ERR;
      if (nc_def_var(ncid, VAR1_NAME, NC_INT, 2, dimids, &varid)) ERR;
      if (nc_put_att_text(ncid, varid, ATT_TEXT_NAME, strlen(speech)+1, speech)) ERR;
      if (nc_close(ncid)) ERR;

      /* Open the file and check attributes. */
      if (nc_open(FILE_NAME, 0, &ncid)) ERR;
    	 if (nc_inq_att(ncid, v, ATT_TEXT_NAME, &att_type, &att_len)) ERR;
    	 if (att_type != NC_CHAR || att_len != strlen(speech) + 1) ERR;
    	 if (!(speech_in = malloc(att_len + 1))) ERR;
    	 if (nc_get_att_text(ncid, 0, ATT_TEXT_NAME, speech_in)) ERR;
    	 if (strcmp(speech, speech_in)) ERR;
    	 free(speech_in);
      if (nc_close(ncid)) ERR;
   SUMMARIZE_ERR;
   FINAL_RESULTS;
}
