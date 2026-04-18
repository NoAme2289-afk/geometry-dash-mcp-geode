# Geometry Dash MCP with Geode SDK

MCP сервер для создания уровней Geometry Dash с использованием Geode SDK для полной автоматизации.

## Архитектура

```
AI (OpenCode/Claude)
    ↓ MCP Protocol
MCP Server (TypeScript)
    ↓ Named Pipe IPC
Geode Mod (C++)
    ↓ Geode SDK API
Geometry Dash
```

## Возможности

- ✅ Создание уровней через AI
- ✅ Автоматическое добавление объектов в редактор
- ✅ Hot-reload в реальном времени
- ✅ Работает с любой версией GD (через Geode)
- ✅ Стабильно и безопасно

## Структура проекта

```
geometry-dash-mcp/
├── mcp-server/          # TypeScript MCP сервер
│   ├── src/
│   │   ├── index.ts
│   │   ├── ipc-client.ts
│   │   └── types.ts
│   └── package.json
├── geode-mod/           # Geode мод (C++)
│   ├── src/
│   │   └── main.cpp
│   ├── mod.json
│   └── CMakeLists.txt
└── .github/
    └── workflows/
        └── build.yml    # GitHub Actions для сборки
```

## Установка

### 1. Установите Geode

Скачайте и установите Geode: https://geode-sdk.org/install

### 2. Соберите мод через GitHub Codespaces

1. Откройте репозиторий на GitHub
2. Нажмите "Code" → "Codespaces" → "Create codespace"
3. Мод соберется автоматически
4. Скачайте `.geode` файл из Artifacts

### 3. Установите мод

Скопируйте `.geode` файл в папку:
```
C:\Users\<USER>\AppData\Local\GeometryDash\geode\mods\
```

### 4. Настройте MCP сервер

Добавьте в OpenCode config (`~/.config/opencode/opencode.jsonc`):

```json
{
  "mcp": {
    "geometry-dash": {
      "type": "local",
      "command": ["node", "path/to/mcp-server/build/index.js"],
      "enabled": true
    }
  }
}
```

## Использование

1. Запустите Geometry Dash с Geode
2. Откройте редактор уровней
3. В OpenCode/Claude напишите:

```
Создай уровень "Test Level"
Добавь блок на (0, 0)
Добавь блок на (1, 0)
Добавь шип на (2, 0)
Сделай hot reload
```

Объекты появятся в редакторе мгновенно!

## Разработка

### Локальная сборка мода (если есть Visual Studio)

```bash
cd geode-mod
mkdir build && cd build
cmake .. -A x64
cmake --build . --config Release
```

### Сборка MCP сервера

```bash
cd mcp-server
npm install
npm run build
```

## Технические детали

### Geode Mod

Мод использует Geode SDK для:
- Получения указателя на `LevelEditorLayer`
- Создания объектов через `GameObject::objectFromString()`
- Добавления объектов в редактор через `addObject()`

### IPC

Связь между MCP сервером и модом через Named Pipe:
- Pipe name: `\\.\pipe\gd_mcp_geode_pipe`
- Формат данных: `LOAD_LEVEL:id,x,y;id,x,y;...`

## Лицензия

MIT

## Авторы

- MCP Server: TypeScript
- Geode Mod: C++ with Geode SDK
- Created: April 2026
