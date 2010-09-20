require 'test_helper'

class ExerciseSetsControllerTest < ActionController::TestCase
  setup do
    @exercise_set = exercise_sets(:one)
  end

  test "should get index" do
    get :index
    assert_response :success
    assert_not_nil assigns(:exercise_sets)
  end

  test "should get new" do
    get :new
    assert_response :success
  end

  test "should create exercise_set" do
    assert_difference('ExerciseSet.count') do
      post :create, :exercise_set => @exercise_set.attributes
    end

    assert_redirected_to exercise_set_path(assigns(:exercise_set))
  end

  test "should show exercise_set" do
    get :show, :id => @exercise_set.to_param
    assert_response :success
  end

  test "should get edit" do
    get :edit, :id => @exercise_set.to_param
    assert_response :success
  end

  test "should update exercise_set" do
    put :update, :id => @exercise_set.to_param, :exercise_set => @exercise_set.attributes
    assert_redirected_to exercise_set_path(assigns(:exercise_set))
  end

  test "should destroy exercise_set" do
    assert_difference('ExerciseSet.count', -1) do
      delete :destroy, :id => @exercise_set.to_param
    end

    assert_redirected_to exercise_sets_path
  end
end
