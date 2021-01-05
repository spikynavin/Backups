#!/bin/bash
echo "GIT Repo creator & GIT Pusher"
echo ""
echo "Enter these Options"
echo "1. Create new repo"
echo "2. GIT Pushing with git init & repos"
echo "3. GIT Pushing with existing gitinit & repos"
echo ""
while :
do
        read string
        case $string in
        1)
                echo "Creat new repo"
                echo ""
                echo "Enter reponame"
                read reponame
                curl -u 'spikynavin:<acess-token>' https://api.github.com/user/repos -d '{"name":"'$reponame'"}' > repo_log
                file="./repo_log"
                grep -i "Bad credentials" $file
                if [ $? -eq 0 ];then
                        echo "Repo not created successfully"
                else
                        echo "Repo created successfully"
                        repotimestamp=`date "+%I-%M-%S %d/%m/%Y"`
                        echo "Reponame:$reponame Created on:$repotimestamp" >> gitlog.txt
			cat gitlog.txt
                fi
        ;;
        2)
                echo "GIT Pushing with git init & repos"
                timestamp=`date "+%I-%M-%S %d/%m/%Y"`
			echo "Enter path to init git"
			read pathint
			dir="$pathint/.git"
                        if [ -d "$dir" ];then
                                echo "Git already initialized"
                        else
			cd $pathint
			touch gitinit && echo 0 > gitinit
			echo "Preparing to push the source to git"
			git init
			echo "Adding files to git"
			git add --all
			#git status
			echo "Enter commit message for your references"
			read commitmsg
			echo "Timestamp: $timestamp commmit message: $commitmsg" >> gitlog.txt
			git commit -m "$commitmsg"
			echo "Enter origin to push"
			read origin
			echo "$origin"
			echo "Enter branch to push"
			read branch
			git push -u $origin $branch
			fi			
	;;
	3)
		echo "GIT Pushing with existing gitinit & repos"
			timestamp=`date "+%I-%M-%S %d/%m/%Y"`
			echo "Enter path to init git"
                        read pathint
                        cd $pathint
                        echo "Preparing to push the source to git"
			echo ""
                        echo "Adding files to git"
                        git add --all
                        #git status
                        echo "Enter commit message for your references"
                        read commitmsg
                        echo "Timestamp: $timestamp commmit message: $commitmsg" >> gitlog.txt
                        git commit -m "$commitmsg"
                        echo "Enter origin to push"
                        read origin
                        echo "$origin"
                        echo "Enter branch to push"
                        read branch
                        git push -u $origin $branch
	;;	
        *)
                echo "Can't understand exit"
                break
        ;;
esac
done
