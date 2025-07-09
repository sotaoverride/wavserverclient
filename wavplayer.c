#include <stdio.h>
#include <stdlib.h>
#include <pulse/simple.h>
#include <pulse/error.h>
// Function to play a WAV file using PulseAudio
void play_wav_with_pulseaudio(const char *filename) {
	pa_simple *s = NULL;
	pa_sample_spec ss;
	int error;

	// Set up the sample specification (adjust as needed for your WAV file)
	ss.format = PA_SAMPLE_S16NE; // Signed 16-bit PCM, native endian
	ss.channels = 2; // Stereo
	ss.rate = 44100; // 44.1 kHz sample rate

	// Create a new connection to the PulseAudio server
	s = pa_simple_new(
			NULL,             // Use the default server
			"WavPlayer",      // Our application's name
			PA_STREAM_PLAYBACK, // Playback stream
			NULL,             // Use the default device
			"WavPlayback",    // Description of our stream
			&ss,              // Our sample format
			NULL,             // Use default channel map
			NULL,             // Use default buffering attributes
			&error            // Ignore error code
			);

	if (!s) {
		fprintf(stderr, "pa_simple_new() failed: %s\n", pa_strerror(error));
		return;
	}

	// Open the WAV file (replace with proper WAV file handling if needed)
	FILE *wav_file = fopen(filename, "rb");
	if (!wav_file) {
		perror("Error opening WAV file");
		pa_simple_free(s);
		return;
	}

	// Read and play the audio data (skip the header, assuming raw PCM after byte 44)
	char buffer[4096];
	size_t bytes_read;

	//fseek(wav_file, 44, SEEK_SET); // Skip WAV header (assuming standard 44-byte header)

	while ( (bytes_read = fread(buffer, 1, sizeof(buffer), wav_file)) > 0 ){
		if (pa_simple_write(s, buffer, bytes_read, &error) < 0) {
			fprintf(stderr, "pa_simple_write() failed: %s\n", pa_strerror(error));
			break;
		}
	}
	/* Make sure that every single sample was played */
	if (pa_simple_drain(s, &error) < 0) {
		fprintf(stderr, __FILE__": pa_simple_drain() failed: %s\n", pa_strerror(error));
	}
	// Close the file and clean up PulseAudio
	fclose(wav_file);
}
int main() {
	FILE *pFile;
	pFile = fopen("output.wav", "wb"); // For wav data
	if (pFile == NULL) {
		// Handle error, e.g., print an error message and exit
		perror("Error opening file");
		return 1;
	}
	char buffer[1024];
	size_t bytes_read;

	// Read from stdin (e.g., piped output from another program)
	while ( (bytes_read = fread(buffer, 1, sizeof(buffer), stdin)) > 0){
		printf("Read from stdin: %s\n", buffer);
		fwrite(buffer, 1, bytes_read, pFile);

	}
	fprintf(stderr, "Error reading from stdin or end of file reached.\n");
	printf("Done Reading from stdin: \n");
	play_wav_with_pulseaudio("output.wav");
	return 0;
}
