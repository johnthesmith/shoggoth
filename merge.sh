#!/bin/bash

# Проверяем, передан ли аргумент для ветки
if [ -z "$1" ]; then
    read -p "Введите имя ветки: " branch
else
    branch=$1
fi

# Проверяем существование ветки
if ! git rev-parse --verify "$branch" >/dev/null 2>&1; then
    echo "Ветка $branch не существует."
    exit 1
fi

# Проверяем, передан ли аргумент для сообщения коммита
if [ -z "$2" ]; then
    read -p "Введите сообщение коммита: " commit_message
    # Если сообщение не указано, использовать "autocommit"
    if [ -z "$commit_message" ]; then
        commit_message="autocommit"
    fi
else
    commit_message=$2
fi

# Переключаемся на указанную ветку
git checkout "$branch" || exit 1

# Найдем точку расхождения с master
base_commit=$(git merge-base master "$branch")
if [ -z "$base_commit" ]; then
    echo "Не удалось найти точку расхождения с master."
    exit 1
fi

# Собираем все коммиты в один
git reset --soft "$base_commit" || exit 1
git commit -m "$commit_message" || exit 1

# Получаем SHA нового коммита
new_commit=$(git rev-parse HEAD)

# Переключаемся на ветку master
git checkout master || exit 1

# Черрипиком переносим коммит в master
git cherry-pick "$new_commit" || exit 1

# Пушим изменения
git push origin master || exit 1

echo "Коммит успешно перенесён в master с сообщением '$commit_message'."


