<script lang="ts">
	import { onMount } from 'svelte';
	import { page } from '$app/state';
	import {
		getCourse,
		getRegisteredCourses,
		registerForCourse,
		markUnitDone,
		isAuthenticated,
		type Course,
		type Registration
	} from '$lib/api';
	import { goto } from '$app/navigation';

	let course_id: string | undefined = $derived(page.params.course_id);
	let course = $state<Course | null>(null);
	let registration = $state<Registration | null>(null);
	let loading = $state(true);
	let error = $state('');

	onMount(async () => {
		if (!isAuthenticated()) {
			goto('/');
			return;
		}
		if (!course_id) {
			error = 'Invalid course ID';
			loading = false;
			return;
		}
		await loadData();
	});

	async function loadData() {
		if (!course_id) return;
		loading = true;
		error = '';
		try {
			const [courseRes, regRes] = await Promise.all([getCourse(course_id), getRegisteredCourses()]);
			const courseData = await courseRes.json();
			const regData = await regRes.json();

			if (courseRes.ok) {
				course = courseData;
			} else {
				error = courseData.error || 'Course not found';
			}

			if (regRes.ok) {
				registration = regData.find((r: Registration) => r.course_id === course_id) || null;
			}
		} catch {
			error = 'Could not connect to server';
		} finally {
			loading = false;
		}
	}

	async function handleRegister() {
		if (!course_id) return;
		const res = await registerForCourse(course_id);
		const data = await res.json();
		if (res.ok) {
			await loadData();
		} else {
			alert(data.error || 'Registration failed');
		}
	}

	async function handleMarkDone(unitId: string) {
		if (!course_id) return;
		const res = await markUnitDone(course_id, unitId);
		const data = await res.json();
		if (res.ok) {
			await loadData();
		} else {
			alert(data.error || 'Failed to mark unit as done');
		}
	}
</script>

<div class="mx-auto max-w-4xl px-4 py-8">
	<a
		href="/courses"
		class="mb-6 inline-block text-sm text-indigo-600 hover:text-indigo-500 dark:text-indigo-400"
		>&larr; Back to courses</a
	>

	{#if loading}
		<div class="py-12 text-center text-gray-500 dark:text-gray-400">Loading course details...</div>
	{:else if error}
		<div class="rounded-lg bg-red-50 p-4 text-sm text-red-600 dark:bg-red-900/30 dark:text-red-400">
			{error}
		</div>
	{:else if course}
		<div
			class="mb-8 rounded-2xl border border-gray-200 bg-white p-8 shadow-sm dark:border-white/10 dark:bg-gray-900"
			style="border-left: 6px solid {course.color || '#3B82F6'}"
		>
			<div class="mb-4 flex items-start justify-between">
				<div>
					<h1 class="text-3xl font-bold text-gray-900 dark:text-white">{course.course_name}</h1>
					<p class="mt-1 text-sm text-gray-500 dark:text-gray-400">{course.course_id}</p>
				</div>
				{#if registration}
					<span
						class="rounded-full bg-green-100 px-3 py-1 text-sm font-medium text-green-700 dark:bg-green-900/30 dark:text-green-400"
					>
						{registration.status}
					</span>
				{/if}
			</div>

			<p class="mb-6 text-gray-600 dark:text-gray-400">{course.description}</p>

			{#if registration}
				<div class="mb-2">
					<div class="flex items-center justify-between text-sm text-gray-500 dark:text-gray-400">
						<span>Course Progress</span>
						<span
							>{Math.round(registration.progress)}% ({registration.units_done}/{registration.units_total}
							units)</span
						>
					</div>
					<div class="mt-1 h-3 w-full rounded-full bg-gray-200 dark:bg-white/10">
						<div
							class="h-3 rounded-full bg-indigo-600 transition-all"
							style="width: {Math.round(registration.progress)}%"
						></div>
					</div>
				</div>
			{:else}
				<button
					onclick={handleRegister}
					class="rounded-lg bg-indigo-600 px-6 py-2.5 text-sm font-semibold text-white hover:bg-indigo-700"
				>
					Register for this course
				</button>
			{/if}
		</div>

		<div>
			<h2 class="mb-4 text-xl font-semibold text-gray-900 dark:text-white">Units</h2>
			{#if course.units && course.units.length > 0}
				<div class="space-y-3">
					{#each course.units as unit (unit.unit_id)}
						<div
							class="flex items-center justify-between rounded-xl border border-gray-200 bg-white p-4 dark:border-white/10 dark:bg-gray-900"
						>
							<div>
								<p class="font-medium text-gray-900 dark:text-white">{unit.unit_name}</p>
								{#if unit.unit_code}
									<p class="text-xs text-gray-500 dark:text-gray-400">{unit.unit_code}</p>
								{/if}
							</div>
							{#if registration}
								<button
									onclick={() => handleMarkDone(unit.unit_id)}
									disabled={registration.status === 'completed'}
									class="cursor-pointer rounded-lg px-4 py-1.5 text-sm font-medium transition-colors disabled:cursor-not-allowed disabled:opacity-50 {registration.status ===
									'completed'
										? 'bg-green-100 text-green-700 dark:bg-green-900/30 dark:text-green-400'
										: 'bg-indigo-100 text-indigo-700 hover:bg-indigo-200 dark:bg-indigo-900/30 dark:text-indigo-400 dark:hover:bg-indigo-900/50'}"
								>
									{registration.status === 'completed' ? 'Completed' : 'Mark Done'}
								</button>
							{/if}
						</div>
					{/each}
				</div>
			{:else}
				<p class="text-sm text-gray-500 dark:text-gray-400">No units found for this course.</p>
			{/if}
		</div>
	{/if}
</div>
