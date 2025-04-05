

👁️‍🗨️ Third Eye - Offline AI Smart Assistant for the Visually Impaired

Third Eye is an AI-powered voice assistant designed to aid visually impaired users by providing real-time audio descriptions of their surroundings using an ESP32-CAM module and a self-hosted AI model running entirely offline.

⸻

🔧 Features
	•	Offline voice-controlled assistant
	•	Real-time object/environment description using images
	•	Smart direction mode for continuous navigation support
	•	Audio feedback using text-to-speech
	•	No internet required – works 100% offline
	•	Developed specifically for visually impaired individuals

⸻

🛠 Technologies Used

ESP32-CAM, Arduino IDE, Python, Flask, gTTS, Google Speech Recognition, Self-hosted LLM model (Ollama), Base64, OpenCV (optional)

⸻

🔌 Hardware Requirements
	•	ESP32-CAM Module
	•	USB to TTL Converter (for programming)
	•	WiFi for local connection (no internet required)
	•	Computer running Python backend with AI model

⸻

🚀 Getting Started

📥 Upload Arduino Code to ESP32-CAM
	1.	Open esp32-cam.ino in Arduino IDE
	2.	Select board: AI Thinker ESP32-CAM
	3.	Enter your WiFi SSID and Password
	4.	Upload the code
	5.	Open Serial Monitor for IP address

💻 Run Backend (Python)
	1.	Clone this repository
	2.	Ensure Ollama + Model is running locally (e.g. ollama run mymodel)
	3.	Run the Python file:

python3 voice_assistant.py



⸻

🎙 Usage
	1.	Say “start” to activate the assistant
	2.	Use natural language prompts like:
	•	“What is in front of me?”
	•	“Read the book”
	•	“Take a photo”
	•	“Which currency is here?”
	3.	To activate direction mode, say:
	•	“guide direction”
	•	It will begin capturing photos continuously and guide movements
	4.	Say “stop direction mode” or “exit” to terminate

⸻

💡 Challenges We Faced
	•	Integrating camera feed with local inference engine
	•	Building an entirely offline voice AI loop
	•	Avoiding delay while maintaining accuracy
	•	Ensuring compatibility between ESP32-CAM and Python backend
	•	Balancing power efficiency and image quality

⸻

🧪 Demo Tested On
	•	ESP32-CAM (AI Thinker)
	•	MacBook M1 with Ollama
	•	Local WiFi network
	•	Flask-based local server
	•	Voice & TTS tested with gTTS + Google Speech Recognition

⸻

🏁 Project Goals
	•	Provide independence and safety for blind individuals
	•	Build a low-cost, locally-deployable assistive system
	•	Ensure all functionality runs offline for maximum privacy
