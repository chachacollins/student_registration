export interface Course {
	course_id: string;
	course_name: string;
	description: string;
	color: string;
	units?: Unit[];
}

export interface Unit {
	unit_id: string;
	unit_name: string;
	unit_code: string;
}

export interface Registration {
	course_id: string;
	course_name: string;
	description: string;
	color: string;
	status: string;
	progress: number;
	units_done: number;
	units_total: number;
}

const API_BASE = 'http://localhost:1234';

function getToken(): string | null {
	if (typeof localStorage === 'undefined') return null;
	return localStorage.getItem('token');
}

export function getStudentId(): string | null {
	if (typeof localStorage === 'undefined') return null;
	return localStorage.getItem('student_id');
}

export function setAuth(token: string, studentId: string) {
	localStorage.setItem('token', token);
	localStorage.setItem('student_id', studentId);
}

export function clearAuth() {
	localStorage.removeItem('token');
	localStorage.removeItem('student_id');
}

export function isAuthenticated(): boolean {
	return getToken() !== null;
}

async function request(path: string, options: RequestInit = {}): Promise<Response> {
	const token = getToken();
	const headers: Record<string, string> = {
		'Content-Type': 'application/json',
		...(options.headers as Record<string, string>)
	};
	if (token) {
		headers['Authorization'] = `Bearer ${token}`;
	}
	return fetch(`${API_BASE}${path}`, { ...options, headers });
}

export async function login(email: string, password: string) {
	return request('/login', {
		method: 'POST',
		body: JSON.stringify({ email, password })
	});
}

export async function registerStudent(data: {
	student_id: string;
	first_name: string;
	last_name: string;
	email: string;
	password: string;
	year_of_study: number;
}) {
	return request('/register', {
		method: 'POST',
		body: JSON.stringify(data)
	});
}

export async function getCourses() {
	return request('/courses');
}

export async function getCourse(courseId: string) {
	return request(`/courses/${courseId}`);
}

export async function registerForCourse(courseId: string) {
	return request('/courses/register', {
		method: 'POST',
		body: JSON.stringify({ course_id: courseId })
	});
}

export async function getRegisteredCourses() {
	return request('/courses/registered');
}

export async function markUnitDone(courseId: string, unitId: string) {
	return request(`/courses/${courseId}/units/${unitId}/done`, {
		method: 'POST'
	});
}
