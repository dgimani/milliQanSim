#!/bin/bash

# Base directories
G4output_dir="/net/cms26/cms26r0/dgimani/milliq_mcgen/G4_output"

#masses=("m_0p1" "m_0p3" "m_0p35" "m_0p4" "m_0p5" "m_0p7" "m_0p9" "m_1p0" "m_1p3" "m_1p5" "m_1p7" "m_2p0" "m_2p3" "m_2p7" "m_3p0" "m_3p5" "m_3p7" "m_4p0" "m_4p5" "m_4p7" "m_5p0" "m_9p0" "m_30p0")
masses=("m_1p5" "m_1p7" "m_2p3" "m_2p7" "m_3p5" "m_3p7" "m_4p0" "m_4p5" "m_30p0")
#masses=("m_1p0")

# Loop over each subdirectory in the propagated directory

date_cutoff="2025-02-7"
for mass_dir in "$G4output_dir"/*; do
  if [ -d "$mass_dir" ]; then
    # Extract the mass (subdirectory name)
    mass=$(basename "$mass_dir")
    #if [[ " ${masses[@]} " =~ " ${mass} " ]]; then
      # Loop over each charge subdirectory inside the mass directory
      for charge_dir in "$mass_dir"/*; do
        if [ -d "$charge_dir" ]; then
          charge=$(basename "$charge_dir")

	  input_dir="${G4output_dir}/${mass}/${charge}/"
	  file_date=$(stat -c %y "${input_dir}/yield.txt" | cut -d' ' -f1)
	   #if [[ "$file_date" < "$date_cutoff" ]]; then
		  echo "Running simAnalysis.py for mass: $mass, charge: $charge"
		  bsub /net/cms17/cms17r0/schmitz/python310/bin/python3.10 simAnalysisSR1.py "$input_dir" 
		  #bsub /net/cms17/cms17r0/schmitz/python310/bin/python3.10 simAnalysisPointingOnly.py "$input_dir" 
	   #fi

        fi
    done
    fi
  #fi
done

