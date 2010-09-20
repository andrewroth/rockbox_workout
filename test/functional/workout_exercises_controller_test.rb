require 'test_helper'

class WorkoutExercisesControllerTest < ActionController::TestCase
  setup do
    @workout_exercise = workout_exercises(:one)
  end

  test "should get index" do
    get :index
    assert_response :success
    assert_not_nil assigns(:workout_exercises)
  end

  test "should get new" do
    get :new
    assert_response :success
  end

  test "should create workout_exercise" do
    assert_difference('WorkoutExercise.count') do
      post :create, :workout_exercise => @workout_exercise.attributes
    end

    assert_redirected_to workout_exercise_path(assigns(:workout_exercise))
  end

  test "should show workout_exercise" do
    get :show, :id => @workout_exercise.to_param
    assert_response :success
  end

  test "should get edit" do
    get :edit, :id => @workout_exercise.to_param
    assert_response :success
  end

  test "should update workout_exercise" do
    put :update, :id => @workout_exercise.to_param, :workout_exercise => @workout_exercise.attributes
    assert_redirected_to workout_exercise_path(assigns(:workout_exercise))
  end

  test "should destroy workout_exercise" do
    assert_difference('WorkoutExercise.count', -1) do
      delete :destroy, :id => @workout_exercise.to_param
    end

    assert_redirected_to workout_exercises_path
  end
end
