# move all commits in to one commit
git merge-base master <branch>
git reset --soft <SHA-1 точки расхождения>
git commit -m "..."

# move commit in to master
git cherry-pick ....hash
git push
