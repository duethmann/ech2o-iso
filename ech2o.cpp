/*******************************************************************************
 * Ech2o, a spatially-distributed, ecohydrologic simulator
 * Copyright (c) 2016 Marco Maneta <marco.maneta@umontana.edu>
 *
 *     This file is part of ech2o, a hydrologic model developed at the 
 *     University of Montana.
 *
 *     Ech2o is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     Ech2o is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Ech2o.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contributors:
 *    Marco Maneta, Sylvain Kuppel
 *******************************************************************************/
/*
 * sativa.cpp
 *
 *  Created on: Feb 19, 2010
 *      Author: Marco Maneta
 */

/*
 * main.cpp
 *
 *  Created on: May 21, 2009
 *      Author: Marco Maneta
 */
#include <time.h>
#include "Sativa.h"

float report_time = 0; //resets to zero when Report_interval time interval passes
float reportMap_time = 0; //resets to zero when ReportMap_interval time interval passes
float advance_climate = 0; // resets to zero when Clim_input_tstep passess

time_t start, theend;
int main(int argc, char* argv[]) {
# ifdef _OPENMP
  printf("Compiled by an OpenMP-compliant implementation.\n");
# endif
  try {
    time(&start);
    Splash(argc, argv);
    CreateWorld(argv);

    while (oControl->current_t_step <= oControl->endtime) {

      //cout << "\nstart time step " << oControl->current_ts_count << "\n";

	  SolveTimeStep();

      CalculateBudgets();

      Report2Screen();

      // Report time series
      report_time += oControl->dt;
      if (report_time >= oControl->report_times) { //if report time overdue
	Report2Ts(); //report results
	report_time = 0; //reset the counter
      }

      // Report maps (only from a certain time step, e.g. to avoid spinup map reporting)
      if (oControl->current_t_step >= oControl->reportMap_start) { 
	reportMap_time += oControl->dt;
	// if (reportMap_time >= oControl->reportMap_times) { //if report time overdue
    if ( (oControl->current_ts_count == 886) || (oControl->current_ts_count == 895) || (oControl->current_ts_count == 918) || (oControl->current_ts_count == 950) || (oControl->current_ts_count == 1207) || (oControl->current_ts_count == 1215) || (oControl->current_ts_count == 1222) || (oControl->current_ts_count == 1223) || (oControl->current_ts_count == 1255) || (oControl->current_ts_count == 1270) || (oControl->current_ts_count == 1271) || (oControl->current_ts_count == 1327) || (oControl->current_ts_count == 1334) || (oControl->current_ts_count == 1335) || (oControl->current_ts_count == 1351) || (oControl->current_ts_count == 1366) || (oControl->current_ts_count == 1599) || (oControl->current_ts_count == 1631) || (oControl->current_ts_count == 1702) || (oControl->current_ts_count == 1703) || (oControl->current_ts_count == 1926) || (oControl->current_ts_count == 1927) || (oControl->current_ts_count == 1935) || (oControl->current_ts_count == 1951) || (oControl->current_ts_count == 1959) || (oControl->current_ts_count == 1967) || (oControl->current_ts_count == 1975) || (oControl->current_ts_count == 1983) || (oControl->current_ts_count == 2023) || (oControl->current_ts_count == 2038) || (oControl->current_ts_count == 2087) || (oControl->current_ts_count == 2287) || (oControl->current_ts_count == 2303) || (oControl->current_ts_count == 2311) || (oControl->current_ts_count == 2367) || (oControl->current_ts_count == 2455) || (oControl->current_ts_count == 2655) || (oControl->current_ts_count == 2663) || (oControl->current_ts_count == 2711) || (oControl->current_ts_count == 2775) || (oControl->current_ts_count == 2815)){
	   Report2Maps(); //report results
	   reportMap_time = 0; //reset the counter
	}
      }

      cout << "\nEnd time step " << oControl->current_ts_count;
      cout << "\nSimulation time " << oControl->current_t_step
	   << " seconds (" << oControl->current_t_step / 86400
	   << " days)\n\n";

      oControl->AdvanceTimeStep();

      advance_climate += oControl->dt;
      if (advance_climate >= oControl->BC_dt) {
	oAtmosphere->AdvanceClimateMaps(*oControl);

	if(oControl->toggle_veg_dyn==2){
	  //	  cout << "\nTry advance LAI maps" << endl;
	  oBasin->AdvanceLAIMaps();
	  //	  cout << "\nDone advance LAI maps" << endl;
	}

	advance_climate = 0;
      }

    }

  } catch (...) {
    cerr
      << "Something bad happened that I cannot really handle until I have a better exception management"
      << endl;
    CrunchWorld();
    return 0;
  }

  CrunchWorld();
  time(&theend);
  int tot_sec = difftime(theend, start);

  int dd = tot_sec / 86400;
  tot_sec = tot_sec % 86400;
  int hh = tot_sec / 3600;
  tot_sec = tot_sec % 3600;
  int mm = tot_sec / 60;
  tot_sec = tot_sec % 60;
  int ss = tot_sec;
  printf("\nTotal run time elapsed:  %i (days) %02i:%02i:%02i (hh:mm:ss)\n", dd,
	 hh, mm, ss);

  return 0;

}
