// Pages/BoardPage.jsx
import React from 'react'
import {
  Box,
  Container,
  Typography,
  Paper,
  Stack,
  Chip,
  Button,
  Divider,
} from '@mui/material'
import { useNavigate } from 'react-router-dom'
import { useGlobal } from '../SessionManager'

export default function BoardPage() {
  const navigate = useNavigate()
  const { state } = useGlobal()
  const board = state.currentTaskBoard

  if (!board) {
    return (
      <Box className="board-page-root">
        <Container maxWidth="lg" className="board-page-container">
          <Box className="board-hero">
            <Typography variant="h4" className="board-title">
              No board selected
            </Typography>
            <Typography variant="body1" className="board-description">
              Please go back to the dashboard and choose a task board.
            </Typography>
            <Box sx={{ mt: 3 }}>
              <Button variant="contained" onClick={() => navigate('/')}>
                Back to dashboard
              </Button>
            </Box>
          </Box>
        </Container>
      </Box>
    )
  }

  const isPersonal = board.type === 'personal'
  const boardDescription =
    board.description ??
    'Organize your personal focus and collaborate on shared priorities.'

  return (
    <Box className="board-page-root">
      <Container maxWidth="lg" className="board-page-container">
        {/* Hero / optional header (matches text above the cards) */}
        <Box className="board-hero">
          <Typography variant="h3" className="board-title">
            {board.name}
          </Typography>
          <Typography variant="body1" className="board-description">
            {boardDescription}
          </Typography>
        </Box>

        {/* Main content layout: left panel + right panel */}
        <Stack className="board-main">
          {/* Left info / actions panel */}
          <Paper elevation={0} className="board-side-panel">
            <Stack className="board-side-chip-row">
              <Chip
                label={isPersonal ? 'Personal Board' : 'Shared Board'}
                size="small"
                className={`board-type-chip ${
                  isPersonal
                    ? 'board-type-chip--personal'
                    : 'board-type-chip--shared'
                }`}
              />
              <Typography className="board-side-tasks-meta">
                {board.tasks.length} task
                {board.tasks.length !== 1 && 's'}
              </Typography>
            </Stack>

            <Divider light className="board-divider" />

            <Box>
              <Typography className="board-focus-title">
                Focus overview
              </Typography>
              <Typography className="board-side-focus">
                You&apos;ve got{' '}
                <span className="board-focus-link">
                  {board.tasks.length} active tasks
                </span>{' '}
                on this board.
              </Typography>
            </Box>

            <Box className="board-side-actions">
              <Button
                variant="contained"
                fullWidth
                className="board-add-button"
              >
                + Add task
              </Button>
              <Button
                variant="text"
                fullWidth
                className="board-back-button"
                onClick={() => navigate('/')}
              >
                ← Back to dashboard
              </Button>
            </Box>
          </Paper>

          {/* Right: task list panel */}
          <Paper elevation={1} className="board-task-panel">
            {/* List header */}
            <Box className="board-task-header">
              <Stack direction="row" spacing={1} alignItems="center">
                <Typography
                  variant="subtitle1"
                  className="board-task-header-title"
                >
                  Task queue
                </Typography>
                <Typography
                  variant="body2"
                  className="board-task-header-subtitle"
                >
                  Drag and drop coming soon
                </Typography>
              </Stack>

              <Stack
                direction="row"
                spacing={1}
                className="board-task-filter-chips"
              >
                <Chip
                  label="All"
                  size="small"
                  className="board-filter-chip board-filter-chip--all"
                />
                <Chip
                  label="This week"
                  size="small"
                  className="board-filter-chip board-filter-chip--week"
                />
              </Stack>
            </Box>

            <Divider className="board-divider" />

            {/* Task cards */}
            <Stack className="board-task-list">
              {board.tasks.map((task, index) => (
                <Paper
                  key={task.id}
                  elevation={0}
                  className="board-task-card"
                  // later: navigate to `/boards/${board.id}/tasks/${task.id}`
                  onClick={() => console.log('Clicked task', task.id)}
                >
                  <Box className="board-task-card-main">
                    <Typography
                      variant="body1"
                      className="board-task-title"
                    >
                      {task.name}
                    </Typography>
                    <Typography
                      variant="caption"
                      className="board-task-subtext"
                    >
                      #{index + 1} • Keep this moving today
                    </Typography>
                  </Box>

                  <Stack
                    direction="row"
                    spacing={1}
                    alignItems="center"
                    className="board-task-meta"
                  >
                    <Chip
                      label={`Due ${task.dueDate}`}
                      size="small"
                      className="board-chip-due"
                    />
                    <Chip
                      label="In progress"
                      size="small"
                      className="board-chip-status"
                    />
                  </Stack>
                </Paper>
              ))}
            </Stack>
          </Paper>
        </Stack>
      </Container>
    </Box>
  )
}