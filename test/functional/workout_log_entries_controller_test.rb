require 'test_helper'

class WorkoutLogEntriesControllerTest < ActionController::TestCase
  setup do
    @workout_log_entry = workout_log_entries(:one)
  end

  test "should get index" do
    get :index
    assert_response :success
    assert_not_nil assigns(:workout_log_entries)
  end

  test "should get new" do
    get :new
    assert_response :success
  end

  test "should create workout_log_entry" do
    assert_difference('WorkoutLogEntry.count') do
      post :create, :workout_log_entry => @workout_log_entry.attributes
    end

    assert_redirected_to workout_log_entry_path(assigns(:workout_log_entry))
  end

  test "should show workout_log_entry" do
    get :show, :id => @workout_log_entry.to_param
    assert_response :success
  end

  test "should get edit" do
    get :edit, :id => @workout_log_entry.to_param
    assert_response :success
  end

  test "should update workout_log_entry" do
    put :update, :id => @workout_log_entry.to_param, :workout_log_entry => @workout_log_entry.attributes
    assert_redirected_to workout_log_entry_path(assigns(:workout_log_entry))
  end

  test "should destroy workout_log_entry" do
    assert_difference('WorkoutLogEntry.count', -1) do
      delete :destroy, :id => @workout_log_entry.to_param
    end

    assert_redirected_to workout_log_entries_path
  end
end
