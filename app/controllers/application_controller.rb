class ApplicationController < ActionController::Base
  protect_from_forgery
  include OpenFlashChart::Controller
  include OpenFlashChart

  # add an initializer with
  #
  # module Auth
  #   USERNAME = 'username'
  #   PASSWORD = 'password'
  # end
  #
  # Add it to #{shared_path}/config/initializers/auth.rb on your server.
  if defined?(Auth)
    before_filter :authenticate
    def authenticate
      authenticate_or_request_with_http_basic do |username, password|
        username == Auth::USERNAME && password == Auth::PASSWORD
      end
    end
  end
end
