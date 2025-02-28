#! /bin/bash

bash_par="$1"
save_par="$2" # "n" or "neutron" inputs can save the RHICf neutron event with RHICf pi0 event

jobNum=120
eventGenNum=7000000

screenName=RHICfSim_PYTHIA8Detroit
pythiaPath=/home/shlee/workspace/rhicf/generators/pythia8
dataPath=/home/shlee/workspace/rhicf/data

export PYTHIA8=/home/public/pythia8310/install
export PYTHIA8DATA=$PYTHIA8/share/Pythia8/xmldoc
export LD_LIBRARY_PATH=$PYTHIA8/lib:$LD_LIBRARY_PATH

if [[ "${bash_par}" == "0" || "${bash_par}" == "TS" ]] || [[ "${bash_par}" == "1" || "${bash_par}" == "TL" ]] || [[ "${bash_par}" == "2" || "${bash_par}" == "TOP" ]] ; 
then
    typeName="-1"
    if [ "${bash_par}" == "TS" ] ;
    then
        typeName="0"

    elif [ "${bash_par}" == "TL" ] ;
    then
        typeName="1"

    elif [ "${bash_par}" == "TOP" ] ;
    then
        typeName="2"
    else 
        typeName=${bash_par}
    fi

    if [ "${save_par}" == "n" ] || [ "${save_par}" == "neutron" ] ;
    then
        save_par=" -option neutron"
    else
        save_par=""
    fi

    for (( i=1; i<jobNum+1; i++ ))
    do
        # compile 
        echo "copy run_RHICfSim_PYTHIA8Detroit_${i}.cc"
        cp ${pythiaPath}/RHICfSimPYTHIA8_Detroit_templete.cc ${pythiaPath}/run_rhicfsim_pythia_${i}.cc
        cd ${pythiaPath} && make run_rhicfsim_pythia_${i}

        # screen excute
        screen -dmS ${screenName}_${i} bash 
        screen -S ${screenName}_${i} -X stuff "cd ${pythiaPath}
"
        screen -S ${screenName}_${i} -X stuff "./run_rhicfsim_pythia_${i} -r ${typeName} -n ${eventGenNum}${save_par} -p ${dataPath}
"           
        echo "RHIfSim::Generators PYTHIA8 job ${i} has completed !!! (run_rhicfsim_pythia_${i} -r ${typeName} -n ${eventGenNum}${save_par} -p ${dataPath})"
    done

else
    # screen section kill
    if [ "${bash_par}" == "screenclear" ] ; 
    then
        for (( i=1; i<jobNum+1; i++ ))
        do
            screen -X -S ${screenName}_${i} quit
            echo "screen kill ${screenName}_${i}"
        done

    # copying file remove
    elif [ "${bash_par}" == "fileclear" ] ; 
    then 

        for files in ${pythiaPath}/run_rhicfsim_pythia_*
        do
            echo "rm ${files}"
            rm ${files}
        done

    # root file remove
    elif [ "${bash_par}" == "rootclear" ] ; 
    then 
        for files in ${dataPath}/RHICfSim_PYTHIA8_Detroit_*
        do
            echo "rm ${files}"
            rm ${files}
        done


    # all clear 
    elif [ "${bash_par}" == "allclear" ] ; 
    then
        for (( i=1; i<jobNum+1; i++ ))
        do
            screen -X -S ${screenName}_${i} quit
            echo "screen kill ${screenName}_${i}"
        done

        for files in ${pythiaPath}/run_rhicfsim_pythia_*
        do
            echo "rm ${files}"
            rm ${files}
        done

        for files in ${dataPath}/RHICfSim_PYTHIA8_Detroit_*
        do
            echo "rm ${files}"
            rm ${files}
        done
    fi


fi





