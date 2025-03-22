#!/bin/bash

# Заголовок таблицы
echo -e "Version\tKey\tFrom\tTo\tCurrent Survival Error Avg"

# Проход по всем папкам в ./ver/*/
for dir in ./ver/*/; do
    # Пути к файлам
    json_file="$dir/net.json"
    commit_file="$dir/mon/commit.json"
    
    # Проверка существования файлов
    if [[ -f "$json_file" && -f "$commit_file" ]]; then
        # Извлекаем version.current
        version=$(jq -r '.version.current' "$json_file")

        # Берем последнюю мутацию из массива
        last_mutation=$(jq -r '.version.mutation | last' "$json_file")

        # Извлекаем ключи мутации
        key=$(echo "$last_mutation" | jq -r '.key')
        from=$(echo "$last_mutation" | jq -r '.from')
        to=$(echo "$last_mutation" | jq -r '.to')

        # Извлекаем currentSurvivalErrorAvg из commit.json
        current_survival_error_avg=$(jq -r '.reason.currentSurvivalErrorAvg' "$commit_file")
        
        # Выводим данные
        echo -e "$version\t$key\t$from\t$to\t$current_survival_error_avg"
    fi
done

