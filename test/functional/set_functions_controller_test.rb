require 'test_helper'

class SetFunctionsControllerTest < ActionController::TestCase
  setup do
    @set_function = set_functions(:one)
  end

  test "should get index" do
    get :index
    assert_response :success
    assert_not_nil assigns(:set_functions)
  end

  test "should get new" do
    get :new
    assert_response :success
  end

  test "should create set_function" do
    assert_difference('SetFunction.count') do
      post :create, :set_function => @set_function.attributes
    end

    assert_redirected_to set_function_path(assigns(:set_function))
  end

  test "should show set_function" do
    get :show, :id => @set_function.to_param
    assert_response :success
  end

  test "should get edit" do
    get :edit, :id => @set_function.to_param
    assert_response :success
  end

  test "should update set_function" do
    put :update, :id => @set_function.to_param, :set_function => @set_function.attributes
    assert_redirected_to set_function_path(assigns(:set_function))
  end

  test "should destroy set_function" do
    assert_difference('SetFunction.count', -1) do
      delete :destroy, :id => @set_function.to_param
    end

    assert_redirected_to set_functions_path
  end
end
