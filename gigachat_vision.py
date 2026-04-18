"""
GigaChat Vision API - Анализ изображений
Использует GigaChat API для анализа изображений
"""

import requests
import base64
import json
import os
from pathlib import Path


class GigaChatVision:
    def __init__(self, api_key):
        """
        Инициализация клиента GigaChat
        
        Args:
            api_key: API ключ от GigaChat (авторизационные данные)
        """
        self.api_key = api_key
        self.base_url = "https://gigachat.devices.sberbank.ru/api/v1"
        self.access_token = None
        
    def get_access_token(self):
        """Получить access token для работы с API"""
        url = "https://ngw.devices.sberbank.ru:9443/api/v2/oauth"
        
        headers = {
            "Authorization": f"Basic {self.api_key}",
            "RqUID": "6f0b1291-c7f3-43c6-bb2e-9f3efb2dc98e",
            "Content-Type": "application/x-www-form-urlencoded"
        }
        
        data = {
            "scope": "GIGACHAT_API_PERS"
        }
        
        try:
            response = requests.post(url, headers=headers, data=data, verify=False)
            response.raise_for_status()
            self.access_token = response.json()["access_token"]
            return self.access_token
        except Exception as e:
            print(f"Ошибка получения токена: {e}")
            return None
    
    def encode_image(self, image_path):
        """
        Кодировать изображение в base64
        
        Args:
            image_path: Путь к изображению
            
        Returns:
            str: Base64 строка изображения
        """
        with open(image_path, "rb") as image_file:
            return base64.b64encode(image_file.read()).decode('utf-8')
    
    def analyze_image(self, image_path, prompt="Опиши что изображено на этой картинке"):
        """
        Анализировать изображение
        
        Args:
            image_path: Путь к изображению
            prompt: Вопрос/запрос к изображению
            
        Returns:
            str: Ответ от GigaChat
        """
        if not self.access_token:
            print("Получаем access token...")
            if not self.get_access_token():
                return "Ошибка: не удалось получить access token"
        
        # Проверяем существование файла
        if not os.path.exists(image_path):
            return f"Ошибка: файл {image_path} не найден"
        
        # Кодируем изображение
        print(f"Кодируем изображение: {image_path}")
        base64_image = self.encode_image(image_path)
        
        # Определяем MIME тип
        ext = Path(image_path).suffix.lower()
        mime_types = {
            '.jpg': 'image/jpeg',
            '.jpeg': 'image/jpeg',
            '.png': 'image/png',
            '.gif': 'image/gif',
            '.webp': 'image/webp'
        }
        mime_type = mime_types.get(ext, 'image/jpeg')
        
        # Формируем запрос
        url = f"{self.base_url}/chat/completions"
        
        headers = {
            "Authorization": f"Bearer {self.access_token}",
            "Content-Type": "application/json"
        }
        
        payload = {
            "model": "GigaChat-Pro",
            "messages": [
                {
                    "role": "user",
                    "content": f"{prompt}\n<img src='data:{mime_type};base64,{base64_image}' />"
                }
            ],
            "temperature": 0.7
        }
        
        try:
            print("Отправляем запрос в GigaChat...")
            response = requests.post(url, headers=headers, json=payload, verify=False)
            response.raise_for_status()
            
            result = response.json()
            answer = result["choices"][0]["message"]["content"]
            return answer
            
        except requests.exceptions.HTTPError as e:
            if e.response.status_code == 401:
                # Токен истек, получаем новый
                print("Токен истек, получаем новый...")
                self.get_access_token()
                return self.analyze_image(image_path, prompt)
            else:
                return f"Ошибка HTTP: {e}\nОтвет: {e.response.text}"
        except Exception as e:
            return f"Ошибка: {e}"


def main():
    """Пример использования"""
    
    # Ваш API ключ
    API_KEY = "MDE5ZGExNjctYTZiOC03MjZmLThmN2EtMWUwZDQ5MzRlODE1OjBlZDA2MzcwLTQyYTItNDkyYy04NzZkLWZhZDcxOTllMTE4NQ=="
    
    # Создаем клиент
    client = GigaChatVision(API_KEY)
    
    # Путь к изображению
    image_path = "703a79ab57ffe64f7043e714d08f98bd.jpg"
    
    # Проверяем существование файла
    if not os.path.exists(image_path):
        print(f"[ERROR] File {image_path} not found in current directory")
        print(f"Current directory: {os.getcwd()}")
        print("\nUsage:")
        print("  python gigachat_vision.py")
        print("\nOr specify image path:")
        print("  python gigachat_vision.py path/to/image.jpg")
        return
    
    print("=" * 60)
    print("GigaChat Vision - Image Analysis")
    print("=" * 60)
    
    # Анализируем изображение
    result = client.analyze_image(
        image_path,
        prompt="Подробно опиши что изображено на этой картинке"
    )
    
    print("\nResult:")
    print("-" * 60)
    print(result)
    print("-" * 60)


if __name__ == "__main__":
    import sys
    
    # Отключаем предупреждения SSL
    import urllib3
    urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)
    
    main()
