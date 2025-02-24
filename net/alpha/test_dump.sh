#!/bin/bash

# Папка с папками, которые надо сканировать
base_dir="./ver"

# Идем по всем папкам в base_dir
for dir in "$base_dir"/*/; do
    # Проверяем, есть ли папка control_dump
    if [ -d "$dir/control_dump" ]; then
        # Извлекаем имя папки
        folder_name=$(basename "$dir")
        
        # Получаем путь к net.json
        net_json="$dir/net.json"
        
        # Ищем первое имя ключа в секции mutation и его from/to
        first_mutation_key="N/A"
        from_value="N/A"
        to_value="N/A"
        if [ -f "$net_json" ]; then
            first_mutation_key=$(jq -r '.version.mutation | keys[0] // "N/A"' "$net_json")
            from_value=$(jq -r ".version.mutation[\"$first_mutation_key\"].from // \"N/A\"" "$net_json")
            to_value=$(jq -r ".version.mutation[\"$first_mutation_key\"].to // \"N/A\"" "$net_json")
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

                # Выводим имя папки, номер, пару строк, ключ из mutation и from/to
                echo "$folder_name $file_number $result_last_line $test_last_line $first_mutation_key $from_value $to_value"
            fi
        done
    fi
done

#
#
##!/bin/bash
#
## Папка с папками, которые надо сканировать
#base_dir="./ver"
#
## Идем по всем папкам в base_dir
#for dir in "$base_dir"/*/; do
#    # Проверяем, есть ли папка control_dump
#    if [ -d "$dir/control_dump" ]; then
#        # Извлекаем имя папки
#        folder_name=$(basename "$dir")
#
#        # Идем по всем файлам *-result.txt в папке control_dump
#        for result_file in "$dir/control_dump"/*-result.txt; do
#            # Получаем имя файла без "-result.txt"
#            base_name="${result_file%-result.txt}"
#            test_file="${base_name}-test.txt"
#
#            # Если существует соответствующий файл *-test.txt
#            if [ -f "$test_file" ]; then
#                # Извлекаем номер из имени файла
#                file_number="${base_name##*/}"
#
#                # Извлекаем последнюю строку из каждого файла
#                result_last_line=$(tail -n 1 "$result_file")
#                test_last_line=$(tail -n 1 "$test_file")
#
#                # Выводим имя папки, номер и пару строк
#                echo "$folder_name $file_number $result_last_line $test_last_line"
#            fi
#        done
#    fi
#done
#
#
#
