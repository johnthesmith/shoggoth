#!/bin/bash

# Папка с папками, которые надо сканировать
base_dir="./ver"

# Идем по всем папкам в base_dir
for dir in "$base_dir"/*/; do
    # Проверяем, есть ли папка control_dump
    if [ -d "$dir/control_dump" ]; then
        # Извлекаем имя папки
        folder_name=$(basename "$dir")
        # Получаем пути к JSON-файлам
        net_json="$dir/net.json"
        commit_json="$dir/mon/commit.json"
        # Инициализируем переменные
        current_version="N/A"
        last_mutation_key="N/A"
        from_value="N/A"
        to_value="N/A"
        sample="N/A"

        # Читаем net.json
        if [ -f "$net_json" ]; then
            current_version=$(jq -r '.version.current // "N/A"' "$net_json")
            mutation_length=$(jq '.version.mutation | length' "$net_json")
            if [ "$mutation_length" -gt 0 ]; then
                last_index=$((mutation_length - 1))
                last_mutation_key=$(jq -r ".version.mutation[$last_index].key // \"N/A\"" "$net_json")
                from_value=$(jq -r ".version.mutation[$last_index].from // \"N/A\"" "$net_json")
                to_value=$(jq -r ".version.mutation[$last_index].to // \"N/A\"" "$net_json")
            fi
        fi

        # Читаем commit.json
        if [ -f "$commit_json" ]; then
            sample=$(jq -r '.reason.currentSurvivalErrorAvg // "N/A"' "$commit_json")
        fi

        # Идем по всем файлам *-result.txt в папке control_dump
        for result_file in "$dir/control_dump"/*-result.txt; do
            # Получаем имя файла без "-result.txt"
            base_name="${result_file%-result.txt}"
            test_file="${base_name}-test.txt"

            # Если существует соответствующий файл *-test.txt
            if [ -f "$test_file" ]; then
                # Извлекаем номер из имени файла
                file_number="${base_name##*/}"
                
                # Извлекаем последнюю строку из каждого файла
                result_last_line=$(tail -n 1 "$result_file")
                test_last_line=$(tail -n 1 "$test_file")

                # Выводим имя папки, номер, строки, sample, версию, последнюю мутацию
                echo "$folder_name $file_number $result_last_line $test_last_line $sample $current_version $last_mutation_key $from_value $to_value"
            fi
        done
    fi
done

