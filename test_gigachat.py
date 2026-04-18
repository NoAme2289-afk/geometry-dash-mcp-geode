"""
Тестовый скрипт для GigaChat Vision
Укажите путь к изображению при запуске
"""

import sys
import os
from gigachat_vision import GigaChatVision


def main():
    # API ключ
    API_KEY = "MDE5ZGExNjctYTZiOC03MjZmLThmN2EtMWUwZDQ5MzRlODE1OjBlZDA2MzcwLTQyYTItNDkyYy04NzZkLWZhZDcxOTllMTE4NQ=="
    
    # Получаем путь к изображению из аргументов
    if len(sys.argv) > 1:
        image_path = sys.argv[1]
    else:
        print("Usage: python test_gigachat.py <path_to_image>")
        print("\nExample:")
        print("  python test_gigachat.py C:\\Users\\danys\\Desktop\\image.jpg")
        print("  python test_gigachat.py image.jpg")
        return
    
    # Проверяем существование файла
    if not os.path.exists(image_path):
        print(f"[ERROR] File not found: {image_path}")
        return
    
    print("=" * 60)
    print("GigaChat Vision - Image Analysis")
    print("=" * 60)
    print(f"Image: {image_path}")
    print()
    
    # Создаем клиент
    client = GigaChatVision(API_KEY)
    
    # Анализируем изображение
    result = client.analyze_image(
        image_path,
        prompt="Podrobno opishi chto izobrazheno na etoy kartinke"
    )
    
    print("\nResult:")
    print("-" * 60)
    print(result)
    print("-" * 60)


if __name__ == "__main__":
    # Отключаем предупреждения SSL
    import urllib3
    urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)
    
    main()
