# GDMCP - AI-Powered Level Creation

**Geometry Dash MCP** - революционная система для создания уровней через AI и естественный язык!

## 🌟 Возможности

### 60+ Команд
- **8 продвинутых триггеров** - Pulse, Color, Follow, Spawn, Stop, Toggle, Shake, Animate
- **Специальные объекты** - Text, Item Counter, Collision Block, Teleport Portal
- **Генератор паттернов** - Line, Circle, Grid, Wave
- **Batch операции** - массовое создание и управление

### 🔧 Профессиональные инструменты
- **Level Optimizer** - автоматическая оптимизация уровня
- **Auto-backup** - автосохранение каждые 5 минут
- **Version Control** - система версий с откатом
- **Collaboration Tools** - экспорт/импорт секций, merge уровней

### 🤖 AI Интеграция
- Создавайте уровни через естественный язык
- AI анализ играбельности и дизайна
- Итеративное улучшение на основе рекомендаций

## 📦 Установка

1. Установите Python MCP сервер:
```bash
pip install git+https://github.com/NoAme2289-afk/geometry-dash-mcp-geode.git
```

2. Настройте OpenCode (или другой MCP клиент):
```json
{
  "mcpServers": {
    "geometry-dash": {
      "command": "python",
      "args": ["-m", "mcp_server_gd"]
    }
  }
}
```

3. Откройте редактор уровня в GD и нажмите кнопку **MCP**

## 🎮 Использование

Просто опишите что хотите создать:
- "Create a platformer level with moving platforms"
- "Add pulse triggers to make the level more dynamic"
- "Optimize the level and create a backup"

## 🔗 Ссылки

- [GitHub](https://github.com/NoAme2289-afk/geometry-dash-mcp-geode)
- [Документация](https://github.com/NoAme2289-afk/geometry-dash-mcp-geode#readme)
- [Releases](https://github.com/NoAme2289-afk/geometry-dash-mcp-geode/releases)

## ✨ Протестировано

На профессиональном уровне **Viteral666 (aka Okira)**:
- 17,969 объектов
- 1,434 триггера
- 4 года разработки

---

**Версия:** 2.1.0  
**Разработчик:** NoAme2289  
**Лицензия:** MIT
