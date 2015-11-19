// SocketIO.h

#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

using std::string;
using std::map;
using sio::client;
using msclr::interop::marshal_as;

namespace SocketIO {

	generic <typename ActionType>
		public ref class GenericActionRunner
		{
		private:
			sio::socket *_socket;
			Action<ActionType>^ _action;
			GCHandle _garbageCollectorHandle;

			//delegate void CallBack(ActionType type);
			delegate void CallBack(sio::event& ev);

			void Run(ActionType type)
			{
				_action(type);
			}

			void EventHandler(sio::event& ev)
			{
				auto converted = Convert(ev);
				Run(converted);
			}

			ActionType Convert(sio::event& ev)
			{
				auto instance = Activator::CreateInstance<ActionType>();

				// TODO: Populate properties
				auto msg = ev.get_message();

				int test;
				switch (msg->get_flag())
				{
				case sio::message::flag_array:
					test = 1;
				case sio::message::flag_binary:
					test = 2;
				case sio::message::flag_double:
					test = 3;
				case sio::message::flag_integer:
					test = 4;
				case sio::message::flag_object:
					test = 5;
				case sio::message::flag_string:
					test = 6;
				}

				return instance;
				//throw gcnew NotImplementedException("Need to figure out how to convert types here.");
			}

		public:
			GenericActionRunner(String^ eventName, sio::socket* socket, Action<ActionType>^ action)
			{
				_socket = socket;
				_action = action;

				auto _callback = gcnew CallBack(this, &SocketIO::GenericActionRunner<ActionType>::EventHandler);
				_garbageCollectorHandle = GCHandle::Alloc(_callback);
				auto _callbackPointer = Marshal::GetFunctionPointerForDelegate(_callback);
				auto _marshaled_delegate = static_cast<sio::GenericReverseActionRunner::PCallBack>(_callbackPointer.ToPointer());

				GC::Collect();

				auto _runner = new sio::GenericReverseActionRunner(marshal_as<string>(eventName), *_socket, _marshaled_delegate);

				//auto _callback = gcnew CallBack(this, &SocketIO::GenericActionRunner<ActionType>::Run);
				//_garbageCollectorHandle = GCHandle::Alloc(_callback);
				//auto _callbackPointer = Marshal::GetFunctionPointerForDelegate(_callback);
				//auto _marshaled_delegate = static_cast<sio::GenericReverseActionRunner<ActionType>::PCallBack>(_callbackPointer.ToPointer());

				/*GC::Collect();

				auto _runner = new sio::ReverseActionRunner(marshal_as<string>(eventName), *_socket, _marshaled_delegate);*/
			}
		};

		public ref class Socket
		{
		private:
			sio::socket *_socket;
			Action^ _action;
			GCHandle _garbageCollectorHandle;

			delegate void CallBack();

			void Run()
			{
				_action();
			}

		public:
			Socket(sio::socket::ptr socket)
			{
				_socket = socket.get();
			}

			~Socket()
			{
				_garbageCollectorHandle.Free();
				_socket->~socket();
				delete _socket;
			}

			// The following should be moved to the socket class
			void Emit(String ^eventName)
			{
				_socket->emit(marshal_as<string>(eventName));
			}

			void Emit(String ^eventName, Object ^message)
			{
				// TODO: convert message to sio::message::list
				sio::message::list msg;
				//sio::object_message obj;

				_socket->emit(marshal_as<string>(eventName), msg);
			}

			void On(String ^eventName, System::Action ^action)
			{
				_action = action;

				auto _callback = gcnew CallBack(this, &SocketIO::Socket::Run);
				_garbageCollectorHandle = GCHandle::Alloc(_callback);
				auto _callbackPointer = Marshal::GetFunctionPointerForDelegate(_callback);
				auto _marshaled_delegate = static_cast<sio::ReverseActionRunner::PCallBack>(_callbackPointer.ToPointer());

				GC::Collect();

				auto _runner = new sio::ReverseActionRunner(marshal_as<string>(eventName), *_socket, _marshaled_delegate);
			}

			generic <typename T>
				void On(String ^eventName, Action<T> ^action)
				{
					auto _runner = gcnew GenericActionRunner<T>(eventName, _socket, action);
					// auto _runner = gcnew GenericActionRunner<T>(*_socket, action);
				}
		};

		public ref class Client
		{
		private:
			client *_client;

		public:
			~Client()
			{
				_client->~client();
				delete _client;
			}

			property bool Opened
			{
				bool get()
				{
					return _client->opened();
				}
			}

			property String ^SessionId
			{
				String ^get()
				{
					return marshal_as<String^>(_client->get_sessionid());
				}
			}

			void Connect(String ^url)
			{
				_client = new client();
				_client->connect(marshal_as<string>(url));
			}

			void Connect(String ^url, Dictionary<String^, String^>^ query)
			{
				_client = new client();
				map<string, string> convertedQuery;
				for each (auto pair in query)
					convertedQuery[marshal_as<string>(pair.Key)] = marshal_as<string>(pair.Value);

				_client->connect(marshal_as<string>(url), convertedQuery);
			}

			void Close()
			{
				_client->close();
			}

			SocketIO::Socket^ GetSocket()
			{
				return gcnew SocketIO::Socket(_client->socket());
			}

			SocketIO::Socket^ GetSocket(String ^name)
			{
				return gcnew SocketIO::Socket(_client->socket(marshal_as<string>(name)));
			}

			void SetReconnectAttempts(int attempts)
			{
				_client->set_reconnect_attempts(attempts);
			}

			void SetReconnectDelay(int millis)
			{
				_client->set_reconnect_delay(millis);
			}

			void SetReconnectDelayMax(int millis)
			{
				_client->set_reconnect_delay_max(millis);
			}

			void SyncClose()
			{
				_client->sync_close();
			}

			void ClearConnectionListeners()
			{
				_client->clear_con_listeners();
			}

			void ClearSocketListeners()
			{
				_client->clear_socket_listeners();
			}

			void UnimplementedItems()
			{
				/*
				_client->set_close_listener(nullptr);
				_client->set_fail_listener(nullptr);
				_client->set_open_listener(nullptr);
				_client->set_reconnecting_listener(nullptr);
				_client->set_reconnect_listener(nullptr);
				_client->set_socket_close_listener(nullptr);
				_client->set_socket_open_listener(nullptr);
				*/
			}
		};
}
