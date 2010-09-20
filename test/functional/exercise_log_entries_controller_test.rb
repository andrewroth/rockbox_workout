require 'test_helper'

class ExerciseLogEntriesControllerTest < ActionController::TestCase
  setup do
    @exercise_log_entry = exercise_log_entries(:one)
  end

  test "should get index" do
    get :index
    assert_response :success
    assert_not_nil assigns(:exercise_log_entries)
  end

  test "should get new" do
    get :new
    assert_response :success
  end

  test "should create exercise_log_entry" do
    assert_difference('ExerciseLogEntry.count') do
      post :create, :exercise_log_entry => @exercise_log_entry.attributes
    end

    assert_redirected_to exercise_log_entry_path(assigns(:exercise_log_entry))
  end

  test "should show exercise_log_entry" do
    get :show, :id => @exercise_log_entry.to_param
    assert_response :success
  end

  test "should get edit" do
    get :edit, :id => @exercise_log_entry.to_param
    assert_response :success
  end

  test "should update exercise_log_entry" do
    put :update, :id => @exercise_log_entry.to_param, :exercise_log_entry => @exercise_log_entry.attributes
    assert_redirected_to exercise_log_entry_path(assigns(:exercise_log_entry))
  end

  test "should destroy exercise_log_entry" do
    assert_difference('ExerciseLogEntry.count', -1) do
      delete :destroy, :id => @exercise_log_entry.to_param
    end

    assert_redirected_to exercise_log_entries_path
  end
end
