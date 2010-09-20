require 'test_helper'

class SetLogEntriesControllerTest < ActionController::TestCase
  setup do
    @set_log_entry = set_log_entries(:one)
  end

  test "should get index" do
    get :index
    assert_response :success
    assert_not_nil assigns(:set_log_entries)
  end

  test "should get new" do
    get :new
    assert_response :success
  end

  test "should create set_log_entry" do
    assert_difference('SetLogEntry.count') do
      post :create, :set_log_entry => @set_log_entry.attributes
    end

    assert_redirected_to set_log_entry_path(assigns(:set_log_entry))
  end

  test "should show set_log_entry" do
    get :show, :id => @set_log_entry.to_param
    assert_response :success
  end

  test "should get edit" do
    get :edit, :id => @set_log_entry.to_param
    assert_response :success
  end

  test "should update set_log_entry" do
    put :update, :id => @set_log_entry.to_param, :set_log_entry => @set_log_entry.attributes
    assert_redirected_to set_log_entry_path(assigns(:set_log_entry))
  end

  test "should destroy set_log_entry" do
    assert_difference('SetLogEntry.count', -1) do
      delete :destroy, :id => @set_log_entry.to_param
    end

    assert_redirected_to set_log_entries_path
  end
end
