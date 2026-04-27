.RECIPEPREFIX = >

# Compiler
CXX := clang++



# Определяем режим через make goals
ifeq ($(filter debug,$(MAKECMDGOALS)),debug)
    MODE := debug
else
    MODE := release
endif



ifeq ($(MODE),debug)
    CXXFLAGS := -g -O0 -DDEBUG -fsanitize=address -fsanitize=undefined -DTHREAD_PROTECTED -MMD -fPIC -MP -Wall -Wextra
else
    CXXFLAGS := -Os -DNDEBUG -MMD -flto -fPIC -MP -ffunction-sections -fdata-sections
    LDFLAGS := -Wl,--gc-sections -s
endif
LDLIBS := -lpthread -lsqlite3



# Определение объектной директории
OBJ_DIR := ./obj/$(MODE)



vpath %.cpp \
./ \
../../lib/core \
../../lib/json \
../../lib/sock \
../../lib/graph \
src/shoggoth \
src/shoggoth/limb \
src/app \
src/app/loader \
src/app/server \
src/app/client \
src/app/processor \
src/app/teacher \
src/app/evolution \
src/app/debugger


# All sources for objects index
SRCS := \
	$(wildcard ../../lib/core/*.cpp) \
	$(wildcard ../../lib/json/*.cpp) \
	$(wildcard ../../lib/sock/*.cpp) \
	$(wildcard ../../lib/graph/*.cpp) \
	$(wildcard src/shoggoth/*.cpp) \
	$(wildcard src/shoggoth/limb/*.cpp) \
	$(wildcard src/app/server/*.cpp) \
	$(wildcard src/app/client/*.cpp) \
	$(wildcard src/app/*.cpp) \
	$(wildcard src/app/loader/*.cpp) \
	$(wildcard src/app/processor/*.cpp) \
	$(wildcard src/app/teacher/*.cpp) \
	$(wildcard src/app/evolution/*.cpp) \
	$(wildcard src/app/debugger/*.cpp)

# $(info SRCS: $(SRCS))

# Создаем явные зависимости для каждого объектного файла
$(foreach src,$(SRCS),$(eval $(OBJ_DIR)/$(notdir $(src:.cpp=.o)): $(src)))

# Все объектные файлы
OBJS := $(addprefix $(OBJ_DIR)/,$(notdir $(SRCS:.cpp=.o)))
# $(info OBJS: $(OBJS))


DEPFILES := $(OBJS:.o=.d)



# Базовые объекты (общие для всех)
BASE_OBJS := \
    $(OBJ_DIR)/utils.o \
	$(OBJ_DIR)/real.o \
	$(OBJ_DIR)/result.o \
	$(OBJ_DIR)/log_manager.o \
	$(OBJ_DIR)/log.o \
	$(OBJ_DIR)/heap.o \
	$(OBJ_DIR)/mon.o \
	$(OBJ_DIR)/moment.o \
	$(OBJ_DIR)/math.o \
	$(OBJ_DIR)/rnd.o \
	$(OBJ_DIR)/str.o \
	$(OBJ_DIR)/text.o \
	$(OBJ_DIR)/buffer_to_hex.o \
	$(OBJ_DIR)/chain_d1.o \
	$(OBJ_DIR)/buffer_d1.o \
	$(OBJ_DIR)/chain_item_d1.o \
	$(OBJ_DIR)/chain_item.o \
	$(OBJ_DIR)/chain.o \
	$(OBJ_DIR)/chart_data.o \
	$(OBJ_DIR)/chart_list.o \
	$(OBJ_DIR)/json.o \
	$(OBJ_DIR)/param.o \
	$(OBJ_DIR)/param_list.o \
	$(OBJ_DIR)/param_list_file.o \
	$(OBJ_DIR)/sock.o \
	$(OBJ_DIR)/sock_manager.o \
	$(OBJ_DIR)/sock_buffer_item.o \
	$(OBJ_DIR)/sock_buffer.o \
	$(OBJ_DIR)/sock_rpc.o \
	$(OBJ_DIR)/rpc_client.o \
	$(OBJ_DIR)/rpc_server.o \
	$(OBJ_DIR)/point3i.o \
	$(OBJ_DIR)/point3.o \
	$(OBJ_DIR)/point4d.o \
	$(OBJ_DIR)/matrix.o \
	$(OBJ_DIR)/gauss.o \
	$(OBJ_DIR)/param_point.o \
	$(OBJ_DIR)/application.o \
	$(OBJ_DIR)/payload.o \
	$(OBJ_DIR)/payload_engine.o \
	$(OBJ_DIR)/func.o \
	$(OBJ_DIR)/net.o \
	$(OBJ_DIR)/net_config.o \
	$(OBJ_DIR)/nerve_list.o \
	$(OBJ_DIR)/nerve.o \
	$(OBJ_DIR)/limb.o \
	$(OBJ_DIR)/weights_exchange.o \
	$(OBJ_DIR)/layer_list.o \
	$(OBJ_DIR)/layer.o \
	$(OBJ_DIR)/shoggoth_consts.o \
	$(OBJ_DIR)/shoggoth_application.o \
	$(OBJ_DIR)/shoggoth_rpc_server.o \
	$(OBJ_DIR)/shoggoth_rpc_client.o \
	$(OBJ_DIR)/shoggoth_db.o

# Уникальные объекты для каждой программы
SHOGGOTH_EXTRA := \
	$(OBJ_DIR)/shoggoth.o \
	$(OBJ_DIR)/thread_manager.o

EVOLUTION_EXTRA := \
	$(OBJ_DIR)/evolution_application.o \
	$(OBJ_DIR)/evolution_payload.o \
	$(OBJ_DIR)/evolution.o

SERVER_EXTRA := \
	$(OBJ_DIR)/server_payload.o \
	$(OBJ_DIR)/server.o

CLIENT_EXTRA := \
	$(OBJ_DIR)/client_payload.o \
	$(OBJ_DIR)/client.o

LOADER_EXTRA := \
	$(OBJ_DIR)/loader.o \
	$(OBJ_DIR)/loader_payload.o

TEACHER_EXTRA := \
	$(OBJ_DIR)/hid.o \
	$(OBJ_DIR)/rgba.o \
	$(OBJ_DIR)/bitmap.o \
	$(OBJ_DIR)/teacher.o \
	$(OBJ_DIR)/layer_teacher.o \
	$(OBJ_DIR)/limb_teacher.o \
	$(OBJ_DIR)/teacher_payload.o

DEBUGGER_EXTRA := \
	$(OBJ_DIR)/debugger.o \
	$(OBJ_DIR)/debugger_application.o \
	$(OBJ_DIR)/debugger_payload.o \
	$(OBJ_DIR)/terminal.o

PROCESSOR_EXTRA := \
	$(OBJ_DIR)/hid.o \
	$(OBJ_DIR)/processor.o \
	$(OBJ_DIR)/processor_payload.o \
	$(OBJ_DIR)/limb_processor.o \
	$(OBJ_DIR)/layer_processor.o \
	$(OBJ_DIR)/thread_manager.o


# Правило компиляции любого .cpp в .o
$(OBJ_DIR)/%.o: %.cpp
> @mkdir -p $(@D)
> $(CXX) $(CXXFLAGS) -c $< -o $@



# Включение зависимостей от заголовков
-include $(DEPFILES)



# Фиктивные цели
.PHONY: release debug clean

# Цели линковки

release: shoggoth loader.so server.so client.so processor.so teacher.so debugger evolution
> @echo "Release build done"
> @if command -v upx >/dev/null 2>&1; then \
>   echo "Packing with UPX..."; \
>   upx --best --lzma shoggoth evolution debugger 2>/dev/null || true; \
>   upx --best --lzma loader.so server.so client.so processor.so teacher.so 2>/dev/null || true; \
> else \
>   echo "UPX not installed, skipping"; \
> fi

debug: shoggoth loader.so server.so client.so processor.so teacher.so debugger evolution
> @echo "Debug build done"

shoggoth: $(BASE_OBJS) $(SHOGGOTH_EXTRA)
> $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

loader.so: $(BASE_OBJS) $(LOADER_EXTRA)
> $(CXX) $(CXXFLAGS) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS)

processor.so: $(BASE_OBJS) $(PROCESSOR_EXTRA)
> $(CXX) $(CXXFLAGS) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS)

server.so: $(BASE_OBJS) $(SERVER_EXTRA)
> $(CXX) $(CXXFLAGS) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS)

client.so: $(BASE_OBJS) $(CLIENT_EXTRA)
> $(CXX) $(CXXFLAGS) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS)

teacher.so: $(BASE_OBJS) $(TEACHER_EXTRA)
> $(CXX) $(CXXFLAGS) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS)

evolution: $(BASE_OBJS) $(EVOLUTION_EXTRA)
> $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

debugger: $(BASE_OBJS) $(DEBUGGER_EXTRA)
> $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS) -lncursesw

clean:
> rm -rf shoggoth \
loader.so \
server.so \
client.so \
processor.so \
teacher.so \
debugger \
evolution \
obj/
