#ifndef HTTP_LIB
 #include "./http.hpp"
#endif
namespace oauth2{
	class Client{
		protected:
			HttpRequest http;
		public:
			Client();

			void authorizate(const char *userid, const char *pwd); //запрос авторизации
			
			char *token;
			char *refreshToken;
	};
	class ResourseServer{
		protected:
			
		public:
			
	};
	class AuthorizationServer{
		protected:

		public:

	};
};